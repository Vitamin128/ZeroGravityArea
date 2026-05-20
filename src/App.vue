<template>
  <div class="app-root">
    <!-- 全局顶部导航栏 -->
    <NavBar />

    <!-- 路由出口 -->
    <main class="main-content">
      <router-view />
    </main>
  </div>
</template>

<script setup lang="js">
import NavBar from './components/NavBar.vue';
</script>

<style>
/* 全局样式重置 */
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

/* 全局样式 */
body {
  margin: 0;
  min-height: 100vh;
  background: #FFFEF9;
  background-image:
    /* 上层：规则网格线 */
    /* 垂直网格线 */
    repeating-linear-gradient(
      90deg,
      transparent,
      transparent 50px,
      rgba(212, 196, 176, 0.4) 50px,
      rgba(212, 196, 176, 0.4) calc(50px + 1.5px)
    ),
    /* 水平网格线 */
    repeating-linear-gradient(
      0deg,
      transparent,
      transparent 50px,
      rgba(212, 196, 176, 0.4) 50px,
      rgba(212, 196, 176, 0.4) calc(50px + 1.5px)
    ),
    /* 底层：横向木纹纹理 */
    /* 主木纹（较宽的条纹） */
    repeating-linear-gradient(
      0deg,
      rgba(212, 196, 176, 0.15) 0px,
      rgba(212, 196, 176, 0.15) 3px,
      transparent 3px,
      transparent 8px,
      rgba(212, 196, 176, 0.1) 8px,
      rgba(212, 196, 176, 0.1) 12px,
      transparent 12px,
      transparent 25px
    ),
    /* 细木纹（更细腻的纹理） */
    repeating-linear-gradient(
      0deg,
      rgba(212, 196, 176, 0.08) 0px,
      rgba(212, 196, 176, 0.08) 1px,
      transparent 1px,
      transparent 4px
    );
  font-family: 'Lora', 'Crimson Text', 'Georgia', serif;
  color: #2C2C2C;
  overflow-x: hidden;
  font-size: 16px;
  line-height: 1.6;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
}

html {
  font-size: 16px;
}

/* 标题使用无衬线字体 */
h1, h2, h3, h4, h5, h6 {
  font-family: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
  font-weight: 600;
  line-height: 1.3;
}

.app-root {
  min-height: 100vh;
  display: flex;
}

/* 主内容区域：为侧边栏留出空间 */
.main-content {
  flex: 1;
  margin-left: 80px; /* 侧边栏宽度 */
  padding: 40px 60px;
  transition: margin-left 0.3s ease;
}

/* 当侧边栏展开时 */
.sidebar-open .main-content {
  margin-left: 280px;
}

/* 链接样式 */
a {
  color: #8B6F47;
  text-decoration: none;
  transition: all 0.2s ease;
}

a:hover {
  color: #6B5437;
}

/* 按钮基础样式 */
button {
  font-family: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
  cursor: pointer;
  transition: all 0.2s ease;
}

/* 输入框基础样式 */
input, textarea {
  font-family: 'Lora', 'Crimson Text', 'Georgia', serif;
}

/* 滚动条样式 */
::-webkit-scrollbar {
  width: 8px;
  height: 8px;
}

::-webkit-scrollbar-track {
  background: #F7F4ED;
}

::-webkit-scrollbar-thumb {
  background: #D4C4B0;
  border-radius: 4px;
}

::-webkit-scrollbar-thumb:hover {
  background: #8B6F47;
}
</style>
