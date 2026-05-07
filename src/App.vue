<template>
  <div class="app-container">
    <vue-particles id="tsparticles" :options="particlesOptions" />
    
    <div class="search-wrapper">
      <h1 class="title">Zero Gravity Search</h1>
      <SearchBar 
        @search-start="handleStart" 
        @update-results="handleResults" 
      />
    </div>
    
    <!-- 加载中：显示旋转动画 -->
    <LoadingOverlay :active="isLoading" />

    <!-- 加载完成且有结果：显示卡片 -->
    <div class="cards-wrapper" v-if="!isLoading && searchResults.length > 0">
      <ResultCards :results="searchResults" />
    </div>
  </div>
</template>
<script setup lang="js">
import { ref } from 'vue';
import SearchBar from './components/SearchBar.vue';
import ResultCards from './components/ResultCards.vue';
// 引入自定义加载组件
import LoadingOverlay from './components/LoadingOverlay.vue';

const searchResults = ref([]);
const isLoading = ref(false); // 【调试用】临时改为 true，确认组件是否正常渲染

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
<!-- 全局样式：背景放在 body 上，不能用 scoped（否则会影响 fixed 定位） -->
<style>
body {
  margin: 0;
  min-height: 100vh;
  background: linear-gradient(135deg, #1a1a2e 0%, #16213e 50%, #0f3460 100%);
  font-family: 'Outfit', sans-serif;
  color: white;
  overflow-x: hidden;
}
</style>

<style scoped>
/* app-container 不再设置 background/overflow，避免创建堆叠上下文限制 fixed 定位 */
.app-container {
  position: relative;
  display: flex;
  flex-direction: column;
  align-items: center;
  min-height: 100vh;
}
/* 粒子背景：改用 fixed，随视口而非容器定位 */
#tsparticles {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  z-index: 0;
  pointer-events: none;
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