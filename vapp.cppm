#pragma leco add_impl impl
export module vapp;

import casein;
import mtx;
import silog;
import sitime;
import sith;
import traits;
import voo;

using traits::is_callable;

export class vapp : public sith::thread {
  volatile bool m_suspended {};
  volatile bool m_resized{};
  sith::run_guard m_run{};

  mtx::mutex m_mutex{};
  mtx::cond m_sus_cond{};
  bool m_init{};

protected:
  void extent_loop(auto fn) {
    sitime::stopwatch time {};
    int count {};

    m_resized = false;
    while (!interrupted() && !m_resized && !m_suspended) {
      count++;
      fn();
    }

    float t = time.millis() / 1000.0f;
    float fps = static_cast<float>(count) / t;
    silog::log(silog::info, "%d frames in %.3f seconds (%.2f FPS average)",
               count, t, fps);

    mtx::lock l { &m_mutex };
    while (!interrupted() && m_suspended) {
      m_sus_cond.wait(&l);
    }
  }
  void extent_loop(voo::queue *q, voo::swapchain_and_stuff &sw, auto fn) {
    extent_loop([&] {
      sw.acquire_next_image();
      fn();
      sw.queue_present(q);
    });
    q->device_wait_idle();
  }

  void main_loop(const char * app_name, is_callable<voo::device_and_queue &> auto fn) {
    voo::device_and_queue dq { app_name, casein::native_ptr };
    while (!interrupted()) fn(dq);
  }
  void main_loop(const char * app_name, is_callable<voo::device_and_queue &, voo::swapchain_and_stuff &> auto fn) {
    main_loop(app_name, [&](auto & dq) {
      voo::swapchain_and_stuff sw { dq };
      fn(dq, sw);
    });
  }
  void ots_loop(voo::device_and_queue & dq, voo::swapchain_and_stuff & sw, auto && fn) {
    auto q = dq.queue();
    extent_loop(q, sw, [&] {
      sw.queue_one_time_submit(q, fn);
    });
  }
  void render_loop(voo::device_and_queue & dq, voo::swapchain_and_stuff & sw, auto && fn) {
    ots_loop(dq, sw, [&](auto cb) {
      auto scb = sw.cmd_render_pass({ cb });
      fn(cb);
    });
  }

  using thread::interrupted;

public:
  vapp();
  virtual ~vapp() = default;
};
