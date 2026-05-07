<template>
  <div class="app-container">
    <vue-particles id="tsparticles" :options="particlesOptions" />
    
    <!-- 加载动画组件 -->
    <loading 
      v-model:active="isLoading"
      :can-cancel="false"
      :is-full-page="true"
      color="#00aeec"
      background-color="#000"
      :opacity="0.7"
    />
    <div class="search-wrapper">
      <h1 class="title">Zero Gravity Search</h1>
      <!-- 监听开始搜索和更新结果两个事件 -->
      <SearchBar 
        @search-start="handleStart" 
        @update-results="handleResults" 
      />
    </div>
    <!-- 当不在加载状态，且有搜索结果时才显示卡片 -->
    <div class="cards-wrapper" v-if="!isLoading && searchResults.length > 0">
      <ResultCards :results="searchResults" />
    </div>
  </div>
</template>
<script setup lang="js">
import { ref } from 'vue';
import SearchBar from './components/SearchBar.vue';
import ResultCards from './components/ResultCards.vue';
// 引入加载组件及其样式
import Loading from 'vue-loading-overlay';
import 'vue-loading-overlay/dist/css/index.css';
const searchResults = ref([]);
const isLoading = ref(false); // 控制加载状态
const handleStart = () => {
  isLoading.value = true;    // 开启加载
  searchResults.value = [];  // 搜索新内容前清空旧结果，确保卡片隐藏
};
const handleResults = (data) => {
  searchResults.value = data; // 存入结果
  // 为了让用户看清加载动画（防止后端返回太快），可以加个小延迟
  setTimeout(() => {
    isLoading.value = false;  // 关闭加载
  }, 500);
};
// 粒子背景的高级配置
const particlesOptions = {
  background: {
    color: { value: "transparent" } // 保持原来的 CSS 渐变底色
  },
  fpsLimit: 120,
  interactivity: {
    events: {
      onClick: { enable: true, mode: "push" }, // 点击时增加粒子
      onHover: { enable: true, mode: "grab" },  // 悬停时产生连线抓取感
    },
    modes: {
      grab: { distance: 140, links: { opacity: 0.5 } },
      push: { quantity: 4 }
    }
  },
  particles: {
    color: { value: "#4facfe" }, // 使用你标题的青蓝色
    links: {
      color: "#4facfe",
      distance: 150,
      enable: true,
      opacity: 0.2,
      width: 1
    },
    move: {
      enable: true,
      speed: 1.2,
      direction: "none",
      random: false,
      straight: false,
      outModes: { default: "out" }
    },
    number: {
      density: { enable: true, area: 800 },
      value: 80
    },
    opacity: {
      value: { min: 0.1, max: 0.5 }
    },
    shape: { type: "circle" },
    size: {
      value: { min: 1, max: 3 }
    }
  },
  detectRetina: true
};
</script>
<style scoped>
.app-container {
  position: relative; /* 必须是 relative 才能让粒子绝对定位在里面 */
  display: flex;
  flex-direction: column;
  align-items: center;
  min-height: 100vh;
  background: linear-gradient(135deg, #1a1a2e 0%, #16213e 50%, #0f3460 100%);
  font-family: 'Outfit', sans-serif;
  color: white;
  overflow-x: hidden; /* 防止粒子溢出产生滚动条 */
}
/* 粒子背景样式 */
#tsparticles {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  z-index: 0; /* 置于背景渐变之上，但内容之下 */
  pointer-events: all; /* 允许粒子捕捉鼠标交互 */
}
/* 确保内容在粒子层之上 */
.search-wrapper, .cards-wrapper {
  position: relative;
  z-index: 1;
}
.search-wrapper {
  text-align: center;
  width: 35%;
  padding: 20px;
  margin-top: 50px;
  margin-bottom: 30px;
}
.cards-wrapper {
  width: 35%;
  padding: 20px;
}
.title {
  font-size: 2.5rem;
  margin-bottom: 2rem;
  background: linear-gradient(to right, #4facfe 0%, #00f2fe 100%);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}
</style>