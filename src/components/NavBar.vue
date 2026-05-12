<template>
  <nav class="navbar">
    <div class="nav-container">
      <!-- <div class="nav-logo">
        <Icon icon="ph:planet-bold" class="logo-icon" />
        <span>Zero Gravity</span>
      </div> -->
      
      <div class="nav-links">
        <router-link to="/search" class="nav-item">
          <Icon icon="ph:magnifying-glass-bold" />
          <span>主页</span>
        </router-link>
        <router-link to="/history" class="nav-item">
          <Icon icon="ph:magnifying-glass-bold" />
          <span>搜索</span>
        </router-link>
        <router-link to="/about" class="nav-item">
          <Icon icon="tabler:alien"/>
          <span>伙伴</span>
        </router-link>
      </div>
      <div class="nav-logo">
        <Icon icon="ph:planet-bold" class="logo-icon" />
        <span>Zero Gravity Area</span>
      </div>

      <div class="nav-links">
        <router-link to="/search1" class="nav-item">
          <Icon icon="ph:magnifying-glass-bold" />
          <span>搜索</span>
        </router-link>
        <router-link to="/history1" class="nav-item">
          <Icon icon="ph:clock-history-bold" />
          <span>历史</span>
        </router-link>
        <router-link to="/about1" class="nav-item">
          <Icon icon="ph:info-bold" />
          <span>关于</span>
        </router-link>
      </div>

      <div class="nav-actions">
        <!-- 预留一个用户头像或设置按钮 -->
        <button class="icon-btn">
          <Icon icon="ph:user-circle-bold" />
        </button>
      </div>
    </div>
  </nav>
</template>

<script setup lang="js">
import { Icon } from '@iconify/vue';
</script>

<style scoped>
.navbar {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 70px;
  z-index: 1000; /* 确保在粒子和内容之上 */
  background: rgba(255, 255, 255, 0.03);
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
  border-bottom: 1px solid rgba(255, 255, 255, 0.1);
  display: flex;
  align-items: center;
}

.nav-container {
  width: 100%;
  max-width: 1200px;
  /* margin: 0 ; */
  padding: 0 20px;
  display: flex;
  /* justify-content: space-between; */
  align-items: center;
}

.nav-logo {
  display: flex;
  align-items: center;
  gap: 10px;
  font-size: 1.6rem;
  font-weight: 700;
  padding-right: 50px;
  padding-left: 130px;
  white-space: nowrap;
  background: linear-gradient(to right, #4facfe 0%, #00f2fe 100%);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}

.logo-icon {
  font-size: 2.5rem;
  color: #4facfe; /* 后备颜色 */
  -webkit-text-fill-color: initial; /* 修复渐变文字影响图标的问题 */
}

.nav-links {
  margin-left: 50px;
  display: flex;
  gap: 110px;
}

.nav-item {
  text-decoration: none;
  color: rgba(255, 255, 255, 0.7);
  display: flex;
  align-items: center;
  gap: 6px;
  font-size: 1.5rem;
  width: 80px;
  white-space: nowrap;
  transition: all 0.3s ease;
  padding: 5px 10px;
  border-radius: 8px;
}

.nav-item:hover {
  color: white;
  background: rgba(255, 255, 255, 0.1);
}

/* 路由激活时的样式 */
.router-link-active {
  color: #4facfe;
  background: rgba(79, 172, 254, 0.1);
}

.nav-actions {
  display: flex;
  align-items: center;
}

.icon-btn {
  background: none;
  border: none;
  color: white;
  font-size: 1.5rem;
  cursor: pointer;
  opacity: 0.7;
  transition: opacity 0.3s;
}

.icon-btn:hover {
  opacity: 1;
}
</style>
