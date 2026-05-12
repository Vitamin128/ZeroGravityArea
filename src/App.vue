<template>
  <div class="app-root">
    <vue-particles id="tsparticles" :options="particlesOptions" />
    <router-view />
  </div>
</template>

<script setup lang="js">
// 粒子背景配置迁移到 App.vue，作为全局背景
const particlesOptions = {
  preset: "stars",
  background: {
    color: { value: "transparent" }
  },
  particles: {
    number: {
      value: 150,
      density: { enable: true, value_area: 800 }
    },
    move: {
      speed: 0.8
    }
  }
};
</script>

<style>
/* 全局样式：保持背景和字体设置 */
body {
  margin: 0;
  min-height: 100vh;
  background: linear-gradient(135deg, #1a1a2e 0%, #16213e 50%, #0f3460 100%);
  font-family: 'Outfit', sans-serif;
  color: white;
  overflow-x: hidden;
  font-size: 15px;
}
html {
  font-size: 15px;
}

/* 粒子背景样式：确保覆盖全屏且在底层 */
#tsparticles {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  z-index: 0;
  pointer-events: none;
}

.app-root {
  /* height: 50vh; */
  position: relative;
  z-index: 1;
    /* background-color: rgba(255, 0, 0, 0.2); /* 加一个半透明红，你会发现红区只占上半屏 */
}
</style>