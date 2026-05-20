<template>
  <nav class="navbar" :class="{ 'sidebar-open': isOpen }">
    <!-- 顶部细条 -->
    <div class="top-bar">
      <!-- Logo -->
      <router-link to="/" class="logo">
        <!-- <Icon icon="ph:planet" class="logo-icon" /> -->
        <WoodIcon height="1em" class="logo-icon"/>
        <span class="logo-text">Wooden Table</span>
      </router-link>

      <!-- 汉堡菜单按钮 -->
      <button class="hamburger" @click="toggleSidebar" :class="{ active: isOpen }">
        <span></span>
        <span></span>
        <span></span>
      </button>
    </div>

    <!-- 侧边栏 -->
    <transition name="sidebar">
      <div v-if="isOpen" class="sidebar-overlay" @click="closeSidebar">
        <div class="sidebar" @click.stop>
          <!-- 侧边栏头部 -->
          <div class="sidebar-header">
            <h2>导航</h2>
            <button class="close-btn" @click="closeSidebar">
              <Icon icon="ph:x" />
            </button>
          </div>

          <!-- 导航链接 -->
          <nav class="nav-links">
            <router-link
              v-for="item in navLinks"
              :key="item.path"
              :to="item.path"
              class="nav-item"
              @click="closeSidebar"
            >
              <Icon :icon="item.icon" class="nav-icon" />
              <span>{{ item.name }}</span>
            </router-link>
          </nav>

          <!-- 底部用户区域 -->
          <div class="sidebar-footer">
            <div class="divider"></div>
            <button class="user-btn" @click="openAuthModal">
              <img :src="userAvatar || defaultAvatar" @error="(e) => e.target.src = defaultAvatar" alt="User" class="user-avatar" />
              <span>{{ isLoggedIn ? '设置' : '登录' }}</span>
            </button>
          </div>
        </div>
      </div>
    </transition>

    <!-- 登录/注册/设置弹窗 -->
    <Teleport to="body">
      <AuthModal v-if="showAuthModal" :initialView="initialAuthView" @close="handleAuthClose" />
    </Teleport>
  </nav>
</template>

<script setup lang="js">
import { ref, watch } from 'vue';
import { useRoute } from 'vue-router';
import { Icon } from '@iconify/vue';
import defaultAvatar from '../assets/white_user.png';
import AuthModal from './auth/AuthModal.vue';
import WoodIcon from '@iconify-vue/fluent-emoji-high-contrast/wood';

const isOpen = ref(false);
const showAuthModal = ref(false);
const isLoggedIn = ref(false);
const initialAuthView = ref('login');
const userAvatar = ref(localStorage.getItem('avatar_url') || '');
const route = useRoute();

const navLinks = [
  { name: 'Game', path: '/gameing', icon: 'ion:game-controller-outline' },
  { name: '搜索', path: '/search', icon: 'ph:magnifying-glass' },
  { name: '伙伴', path: '/aifriend', icon: 'tabler:alien' },
  { name: '消息', path: '/message', icon: 'tabler:message' },
  { name: '视频', path: '/video', icon: 'mingcute:video-line' },
  { name: '个人主页', path: '/mainpage', icon: 'material-symbols:home-outline-rounded' },
];

const toggleSidebar = () => {
  isOpen.value = !isOpen.value;
  document.body.classList.toggle('sidebar-open', isOpen.value);
};

const closeSidebar = () => {
  isOpen.value = false;
  document.body.classList.remove('sidebar-open');
};

const checkLoginStatus = () => {
  const token = localStorage.getItem('auth_token');
  isLoggedIn.value = !!token;
  userAvatar.value = localStorage.getItem('avatar_url') || '';
};

const openAuthModal = () => {
  initialAuthView.value = isLoggedIn.value ? 'settings' : 'login';
  showAuthModal.value = true;
  closeSidebar();
};

const handleAuthClose = () => {
  showAuthModal.value = false;
  checkLoginStatus();
};

// 路由变化时关闭侧边栏
watch(() => route.path, () => {
  closeSidebar();
});

checkLoginStatus();
</script>

<style scoped>
/* 顶部细条 */
.top-bar {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 60px;
  background: #FFFFFF;
  border-bottom: 1px solid #E8E3DA;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0 30px;
  z-index: 1000;
  box-shadow: 0 1px 3px rgba(0, 0, 0, 0.05);
}

/* Logo */
.logo {
  display: flex;
  align-items: center;
  gap: 10px;
  text-decoration: none;
  color: #2C2C2C;
  font-family: 'Inter', sans-serif;
  font-weight: 600;
  font-size: 1.1rem;
  transition: color 0.2s ease;
}

.logo:hover {
  color: #8B6F47;
}

.logo-icon {
  font-size: 1.5rem;
  color: #8B6F47;
}

.logo-text {
  letter-spacing: 0.5px;
}

/* 汉堡菜单按钮 */
.hamburger {
  width: 32px;
  height: 24px;
  background: none;
  border: none;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  padding: 0;
  position: relative;
}

.hamburger span {
  display: block;
  width: 100%;
  height: 2px;
  background: #2C2C2C;
  border-radius: 2px;
  transition: all 0.3s ease;
}

.hamburger:hover span {
  background: #8B6F47;
}

.hamburger.active span:nth-child(1) {
  transform: translateY(11px) rotate(45deg);
}

.hamburger.active span:nth-child(2) {
  opacity: 0;
}

.hamburger.active span:nth-child(3) {
  transform: translateY(-11px) rotate(-45deg);
}

/* 侧边栏遮罩 */
.sidebar-overlay {
  position: fixed;
  top: 0;
  left: 0;
  width: 100vw;
  height: 100vh;
  background: rgba(0, 0, 0, 0.3);
  z-index: 999;
  display: flex;
}

/* 侧边栏 */
.sidebar {
  width: 280px;
  height: 100vh;
  background: #FFFFFF;
  border-right: 1px solid #E8E3DA;
  display: flex;
  flex-direction: column;
  box-shadow: 2px 0 8px rgba(0, 0, 0, 0.08);
}

/* 侧边栏头部 */
.sidebar-header {
  padding: 30px 24px 20px;
  border-bottom: 1px solid #E8E3DA;
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.sidebar-header h2 {
  font-size: 1.3rem;
  color: #2C2C2C;
  font-weight: 600;
}

.close-btn {
  background: none;
  border: none;
  color: #6B6B6B;
  font-size: 1.3rem;
  padding: 4px;
  display: flex;
  align-items: center;
  justify-content: center;
  border-radius: 2px;
}

.close-btn:hover {
  color: #2C2C2C;
  background: #F7F4ED;
}

/* 导航链接 */
.nav-links {
  flex: 1;
  padding: 20px 0;
  overflow-y: auto;
}

.nav-item {
  display: flex;
  align-items: center;
  gap: 14px;
  padding: 14px 24px;
  color: #6B6B6B;
  text-decoration: none;
  font-size: 1rem;
  transition: all 0.2s ease;
  position: relative;
  font-family: 'Inter', sans-serif;
}

.nav-item::before {
  content: '';
  position: absolute;
  left: 0;
  top: 50%;
  transform: translateY(-50%);
  width: 0;
  height: 60%;
  background: #8B6F47;
  border-radius: 0 2px 2px 0;
  transition: width 0.2s ease;
}

.nav-item:hover {
  color: #2C2C2C;
  background: #F7F4ED;
}

.nav-item:hover::before {
  width: 3px;
}

.router-link-active {
  color: #8B6F47;
  background: #F7F4ED;
  font-weight: 500;
}

.router-link-active::before {
  width: 3px;
}

.nav-icon {
  font-size: 1.3rem;
  flex-shrink: 0;
}

/* 侧边栏底部 */
.sidebar-footer {
  padding: 20px 24px;
  border-top: 1px solid #E8E3DA;
}

.divider {
  height: 1px;
  background: #E8E3DA;
  margin-bottom: 16px;
}

.user-btn {
  width: 100%;
  display: flex;
  align-items: center;
  gap: 12px;
  padding: 12px 16px;
  background: none;
  border: 1px solid #E8E3DA;
  border-radius: 4px;
  color: #2C2C2C;
  font-size: 0.95rem;
  font-family: 'Inter', sans-serif;
  transition: all 0.2s ease;
}

.user-btn:hover {
  background: #F7F4ED;
  border-color: #8B6F47;
}

.user-avatar {
  width: 32px;
  height: 32px;
  border-radius: 50%;
  object-fit: cover;
  border: 1px solid #E8E3DA;
}

/* 侧边栏动画 */
.sidebar-enter-active,
.sidebar-leave-active {
  transition: opacity 0.3s ease;
}

.sidebar-enter-active .sidebar,
.sidebar-leave-active .sidebar {
  transition: transform 0.3s ease;
}

.sidebar-enter-from,
.sidebar-leave-to {
  opacity: 0;
}

.sidebar-enter-from .sidebar {
  transform: translateX(-100%);
}

.sidebar-leave-to .sidebar {
  transform: translateX(-100%);
}
</style>
