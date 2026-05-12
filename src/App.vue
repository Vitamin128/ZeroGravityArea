<template>
  <div class="app-root">
    <!-- 全局星空背景 -->
    <vue-particles id="tsparticles" :options="particlesOptions" />
    
    <!-- 全局顶部导航栏 -->
    <NavBar />

    <!-- 路由出口：增加一个包裹层来处理顶部间距 -->
    <main class="main-content">
      <router-view />
    </main>
  </div>
</template>

<script setup lang="js">
import NavBar from './components/NavBar.vue';

// 粒子背景配置
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
/* 全局样式 */
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
  min-height: 100vh;
  position: relative;
  z-index: 1;
}

/* 主内容区域：预留出导航栏的高度 (60px) */
.main-content {
  padding-top: 30px;
  width: 100%;
}
</style>