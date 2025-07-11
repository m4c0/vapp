#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import vee;
import voo;
import vapp;

class pipeline {
  voo::one_quad m_oq;
  vee::pipeline_layout m_pl;
  vee::render_pass m_rp;
  vee::gr_pipeline m_gp;

public:
  explicit pipeline(voo::device_and_queue & dq) :
    m_oq { dq }
  , m_pl { vee::create_pipeline_layout() }
  , m_rp { voo::single_att_render_pass(dq) }
  , m_gp { vee::create_graphics_pipeline({
    .pipeline_layout = *m_pl,
    .render_pass = *m_rp,
    .shaders {
      voo::shader("poc.vert.spv").pipeline_vert_stage(),
      voo::shader("poc.frag.spv").pipeline_frag_stage(),
    },
    .bindings { m_oq.vertex_input_bind() },
    .attributes { m_oq.vertex_attribute(0) },
  }) }
  {}

  void run(vee::command_buffer cb, vee::extent e) {
    vee::cmd_bind_gr_pipeline(cb, *m_gp);
    vee::cmd_set_viewport(cb, e);
    vee::cmd_set_scissor(cb, e);
    m_oq.run(cb, 0);
  }
};

struct shrt : public vapp {
  void run() override {
    main_loop("poc-voo", [&](auto & dq, auto & sw) {
      pipeline p { dq };
      render_loop(dq, sw, [&] {
        auto cb = sw.command_buffer();
        p.run(cb, sw.extent());
      });
    });
  }
};

struct lng : public vapp {
  void run() override {
    main_loop("poc-voo", [&](auto & dq, auto & sw) {
      pipeline p { dq };
      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&] {
          auto crp = sw.cmd_render_pass();
          auto cb = sw.command_buffer();
          p.run(cb, sw.extent());
        });
      });
    });
  }
} t;
