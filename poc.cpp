#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import vee;
import voo;
import vapp;

struct : public vapp {
  void run() {
    main_loop("poc-voo", [&](auto & dq, auto & sw) {
      auto pl = vee::create_pipeline_layout();
      voo::one_quad_render oqr { "poc", &dq, *pl };
      ots_loop(dq, sw, [&](auto cb) {
        oqr.run(cb, sw.extent());
      });
    });
  }
} t;
