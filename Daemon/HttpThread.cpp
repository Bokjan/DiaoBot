#include <sys/syscall.h>
#include <mutex>
#include <stdexcept>
#include <string>
#include "DaemonHeader.hpp"
#include "Log.hpp"
#include "TfcConfigCodec.hpp"

#ifndef MMG_ENABLE_THREADS
#define MG_ENABLE_THREADS 1
#endif
#include "../ThirdParty/mongoose/mongoose.h"

#ifndef HTTP_WORKER_THREAD_NUM
#define HTTP_WORKER_THREAD_NUM 2
#endif

static sock_t sock[2];
static std::mutex socklock;

struct WorkRequest {
  mg_connection *Connection;
  SimpleHttpMessage *Message;
};

struct WorkResult {
  mg_connection *Connection;
  std::string *ResponseBody;
};

namespace DiaoBot {

static void EventHandler(mg_connection *c, int event, void *data) {
  switch (event) {
    case MG_EV_ACCEPT:
      c->user_data = reinterpret_cast<void *>(c);
      break;
    case MG_EV_HTTP_REQUEST: {
      auto msg = new SimpleHttpMessage;
      auto hm = reinterpret_cast<http_message *>(data);
      msg->Body = std::string(hm->body.p, hm->body.len);
      msg->Method = std::string(hm->method.p, hm->method.len);
      msg->QueryString = std::string(hm->query_string.p, hm->query_string.len);
      msg->URI = std::string(hm->uri.p, hm->uri.len);
      WorkRequest req;
      req.Connection = c;
      req.Message = msg;
      if (write(sock[0], &req, sizeof(req)) < 0) {
        LOG("Write sock failed, Connection=0x%p", req.Connection);
      }
      break;
    }
    case MG_EV_CLOSE:
      c->user_data = nullptr;
      break;
    default:;
  }
}

static void OnWorkComplete(mg_connection *c, int event, void *data) {
  auto res = reinterpret_cast<WorkResult *>(data);
  if (c != res->Connection) {
    return;
  }
  if (res->ResponseBody == nullptr) {
    mg_printf(c, "%s",
              "HTTP/1.1 501 Not Implemented\r\nServer: DiaoBotHttp\r\n\r\nNot implemented!\r\n");
  } else {
    mg_printf(c, "%s", "HTTP/1.1 200 OK\r\nServer: DiaoBotHttp\r\n\r\n");
    if (!res->ResponseBody->empty()) {
      mg_printf(c, "%s", res->ResponseBody->c_str());
    }
  }
  c->flags |= MG_F_SEND_AND_CLOSE;
  delete res->ResponseBody;
}

static void *HttpWorkerThread(void *param) {
  LOG("Worker thread started");
  auto mgr = reinterpret_cast<mg_mgr *>(param);
  WorkRequest req;
  while (!IsKilled) {
    int readret;
    do {
      std::lock_guard<std::mutex> lk(socklock);
      readret = read(sock[1], &req, sizeof(req));
    } while (false);
    if (readret < 0) {
      LOG("Read sock failed, tid = %ld", syscall(SYS_gettid));
      continue;
    }
    if (readret != sizeof(req)) {
      continue;
    }
    WorkResult res;
    res.Connection = req.Connection;
    res.ResponseBody = nullptr;
    if (req.Message->URI == "/callback") {
      res.ResponseBody = CallbackHandler(req.Message);
    } else if (req.Message->URI == "/ping") {
      res.ResponseBody = new string("PONG!");
    }
    delete req.Message;
    mg_broadcast(mgr, OnWorkComplete, reinterpret_cast<void *>(&res), sizeof(res));
  }
  LOG("Worker thread stopped, tid = %ld", syscall(SYS_gettid));
  return nullptr;
}

void DoHttpThreadImpl(void) {
  mg_mgr mgr;
  mg_connection *conn;
  if (mg_socketpair(sock, SOCK_STREAM) == 0) {
    throw std::runtime_error("Cannot open mongoose socket pair");
  }
  mg_mgr_init(&mgr, nullptr);
  if (!MainConf.HasKV("root\\http", "port")) {
    throw std::runtime_error("Config item `root\\http\\port` not found!");
  }
  conn = mg_bind(&mgr, MainConf.GetKV("root\\http", "port").c_str(), EventHandler);
  if (conn == nullptr) {
    throw std::runtime_error("Cannot bind mongoose");
  }
  mg_set_protocol_http_websocket(conn);
  for (int i = 0; i < HTTP_WORKER_THREAD_NUM; ++i) {
    mg_start_thread(HttpWorkerThread, &mgr);
  }
  while (!IsKilled) {
    mg_mgr_poll(&mgr, 200);
  }
  mg_mgr_free(&mgr);
  closesocket(sock[0]);
  closesocket(sock[1]);
}

void DoHttpThread(void) {
  LOG("%s", "HttpThread started");
  SetupCryptor();
  DoHttpThreadImpl();
  LOG("%s", "HttpThread terminated");
}

}  // namespace DiaoBot
