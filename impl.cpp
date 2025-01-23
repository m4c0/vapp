module vapp;

import casein;
import mtx;
import sith;

vapp::vapp() {
  casein::handle(casein::CREATE_WINDOW, [this] { m_run = sith::run_guard{this}; });
  casein::handle(casein::RESIZE_WINDOW, [this] { if (!casein::window_live_resize) m_resized = true; });
  casein::handle(casein::QUIT, [this] { m_run = {}; });

  casein::handle(casein::LEAVE_BACKGROUND, [this] {
    mtx::lock l { &m_mutex };
    m_suspended = false;
    m_sus_cond.wake_all();
  });
  casein::handle(casein::ENTER_BACKGROUND, [this] { m_suspended = true; });
}
