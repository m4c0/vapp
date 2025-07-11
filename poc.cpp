#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import vee;
import voo;
import vapp;

struct shrt : public vapp {
  void run() override {
    main_loop("poc-voo", [&](auto & dq, auto & sw) {
      voo::one_quad oq { dq };
      auto pl = vee::create_pipeline_layout();
      auto rp = voo::single_att_render_pass(dq);
      auto gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = *rp,
        .shaders {
          voo::shader("poc.vert.spv").pipeline_vert_stage(),
          voo::shader("poc.frag.spv").pipeline_frag_stage(),
        },
        .bindings { oq.vertex_input_bind() },
        .attributes { oq.vertex_attribute(0) },
      });
      render_loop(dq, sw, [&] {
        auto cb = sw.command_buffer();
        vee::cmd_bind_gr_pipeline(cb, *gp);
        vee::cmd_set_viewport(cb, sw.extent());
        vee::cmd_set_scissor(cb, sw.extent());
        oq.run(cb, 0);
      });
    });
  }
};

struct lng : public vapp {
  void run() override {
    main_loop("poc-voo", [&](auto & dq, auto & sw) {
      voo::one_quad oq { dq };
      auto pl = vee::create_pipeline_layout();
      auto rp = voo::single_att_render_pass(dq);
      auto gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = *rp,
        .shaders {
          voo::shader("poc.vert.spv").pipeline_vert_stage(),
          voo::shader("poc.frag.spv").pipeline_frag_stage(),
        },
        .bindings { oq.vertex_input_bind() },
        .attributes { oq.vertex_attribute(0) },
      });
      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&] {
          auto crp = sw.cmd_render_pass();
          auto cb = sw.command_buffer();
          vee::cmd_bind_gr_pipeline(cb, *gp);
          vee::cmd_set_viewport(cb, sw.extent());
          vee::cmd_set_scissor(cb, sw.extent());
          oq.run(cb, 0);
        });
      });
    });
  }
} t;
