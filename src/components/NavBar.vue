<template>
  <nav class="navbar">
    <div class="nav-container" ref="navContainer">
      <!-- 动态滑块 -->
      <div class="nav-indicator" :style="indicatorStyle"></div>

      <!-- 1. 左侧区域 (绝对等宽) -->
      <div class="nav-section side-left">
        <div class="nav-links">
          <router-link 
            v-for="item in leftLinks" 
            :key="item.path"
            :to="item.path" 
            class="nav-item"
            :ref="el => setItemRef(el, item.path)"
          >
            <Icon :icon="item.icon" />
            <span>{{ item.name }}</span>
          </router-link>
        </div>
      </div>

      <!-- 2. 中间区域 (Logo) -->
      <div class="nav-logo">
        <Icon icon="ph:planet-bold" class="logo-icon" />
        <span>Zero Gravity Area</span>
      </div>

      <!-- 3. 右侧区域 (绝对等宽) -->
      <div class="nav-section side-right">
        <div class="nav-links">
          <router-link 
            v-for="item in rightLinks" 
            :key="item.path"
            :to="item.path" 
            class="nav-item"
            :ref="el => setItemRef(el, item.path)"
          >
            <Icon :icon="item.icon" />
            <span>{{ item.name }}</span>
          </router-link>
        </div>
        
        <!-- 用户操作按钮 -->
        <div class="nav-actions">
          <div class="user-profile-container" 
               @mouseenter="showTooltip = true" 
               @mouseleave="showTooltip = false">
            <button class="icon-btn" @click="showAuthModal = true">
              <img :src="defaultAvatar" alt="User" class="user-avatar" />
            </button>
            
            <Transition name="tooltip-fade">
              <div v-if="showTooltip" class="glass-tooltip-fixed">
                <div class="tooltip-arrow"></div>
                <div class="tooltip-content">
                  <Icon icon="ph:sign-in-bold" />
                  <span>立即登录</span>
                </div>
              </div>
            </Transition>
          </div>
        </div>
      </div>

    </div>
    
    <!-- 登录/注册/找回密码弹窗 -->
    <Teleport to="body">
      <AuthModal v-if="showAuthModal" @close="showAuthModal = false" />
    </Teleport>
  </nav>
</template>

<script setup lang="js">
import { ref, onMounted, watch, reactive } from 'vue';
import { useRoute } from 'vue-router';
import { Icon } from '@iconify/vue';
import defaultAvatar from '../assets/white_user.png';
import AuthModal from './auth/AuthModal.vue';

const showTooltip = ref(false);
const showAuthModal = ref(false);

const route = useRoute();
const navContainer = ref(null);
const itemRefs = reactive({});

const leftLinks = [
  { name: 'Game', path: '/gameing', icon: 'ion:game-controller-outline' },
  { name: '搜索', path: '/search', icon: 'ph:magnifying-glass-bold' },
  { name: '伙伴', path: '/aifriend', icon: 'tabler:alien' },
];

const rightLinks = [
  { name: '消息', path: '/message', icon: 'tabler:message' },
  { name: '视频', path: '/video', icon: 'mingcute:video-line' },
  { name: '个人主页', path: '/mainpage', icon: 'material-symbols:home-outline-rounded' },
];

const setItemRef = (el, path) => {
  if (el) itemRefs[path] = el.$el || el;
};

const indicatorStyle = reactive({
  width: '0px',
  left: '0px',
  opacity: 0
});

const updateIndicator = () => {
  const activePath = route.path;
  const activeEl = itemRefs[activePath];

  if (activeEl && navContainer.value) {
    const containerRect = navContainer.value.getBoundingClientRect();
    const elRect = activeEl.getBoundingClientRect();
    indicatorStyle.width = `${elRect.width}px`;
    indicatorStyle.left = `${elRect.left - containerRect.left}px`;
    indicatorStyle.opacity = 1;
  }
};

onMounted(() => setTimeout(updateIndicator, 100));
watch(() => route.path, () => setTimeout(updateIndicator, 50));
</script>

<style scoped>
.navbar {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 70px;
  z-index: 1000;
  background: rgba(255, 255, 255, 0.03);
  backdrop-filter: blur(12px);
  border-bottom: 1px solid rgba(255, 255, 255, 0.1);
}

.nav-container {
  height: 100%;
  position: relative;
  display: flex;
  align-items: center;
  padding: 0 40px;
}

/* --- 核心布局：左右对称分权 --- */
.nav-section {
  flex: 1; 
  display: flex;
  align-items: center;
}

.side-left {
  justify-content: flex-end; 
  padding-right: 120px; /* 调大左侧到 Logo 的距离 */
}

.side-right {
  justify-content: flex-start; 
  padding-left: 120px; /* 调大右侧到 Logo 的距离 */
  gap: 30px; 
}

.nav-links {
  display: flex;
  gap: 60px; /* 调大链接与链接之间的距离 */
  white-space: nowrap;
}

/* --- Logo 区域 --- */
.nav-logo {
  flex: 0 0 auto;
  display: flex;
  align-items: center;
  gap: 12px;
  font-size: 1.6rem;
  font-weight: 800;
  white-space: nowrap;
  background: linear-gradient(135deg, #00f2fe 0%, #4facfe 100%);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}

.logo-icon {
  font-size: 2.8rem;
  -webkit-text-fill-color: initial;
}

/* --- 滑块与导航项 --- */
.nav-indicator {
  position: absolute;
  height: 45px;
  background: rgba(79, 172, 254, 0.1);
  border: 1px solid rgba(79, 172, 254, 0.2);
  border-radius: 12px;
  transition: all 0.4s cubic-bezier(0.23, 1, 0.32, 1);
  z-index: -1;
}

.nav-item {
  text-decoration: none;
  color: rgba(255, 255, 255, 0.6);
  display: flex;
  align-items: center;
  gap: 8px;
  font-size: 1.1rem;
  padding: 8px 15px;
  border-radius: 12px;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1); /* 更平滑的过渡 */
}

.nav-item:hover {
  color: white;
  transform: translateY(-3px); /* 悬浮时轻微上浮 */
  text-shadow: 0 0 15px rgba(79, 172, 254, 0.5); /* 增加一点微光 */
}

.nav-item:active {
  transform: translateY(-1px); /* 点击时按下的反馈 */
}

.router-link-active {
  color: #4facfe;
  font-weight: 600;
}

.icon-btn {
  background: none;
  border: none;
  color: white;
  font-size: 2.5rem; /* 调大图标尺寸 */
  cursor: pointer;
  display: flex;
  align-items: center;
  transition: all 0.3s ease;
}

.icon-btn:hover {
  transform: scale(1.1);
  color: #4facfe;
  filter: drop-shadow(0 0 8px rgba(79, 172, 254, 0.4));
}

.user-avatar {
  width: 36px;
  height: 36px;
  border-radius: 50%;
  object-fit: cover;
  border: 1.5px solid rgba(255, 255, 255, 0.2);
  transition: all 0.3s ease;
}

.icon-btn:hover .user-avatar {
  border-color: #4facfe;
}

.nav-item :deep(svg) {
  font-size: 1.6rem; /* 你可以根据感觉调整这个数字，比如 1.4rem 或 1.8rem */
  transition: transform 0.3s ease; /* 让图标的大小变化也带点平滑感 */
}
/* 悬停时让图标再稍微变大一点（可选，效果很赞） */
.nav-item:hover :deep(svg) {
  transform: scale(1.1); 
}

/* --- 用户操作相关 --- */
.user-profile-container {
  position: relative;
  display: flex;
  align-items: center;
}

.glass-tooltip-fixed {
  position: absolute;
  top: 130%;
  left: 50%;
  transform: translateX(-50%);
  z-index: 100;
  padding: 10px 16px;
  background: rgba(255, 255, 255, 0.05);
  backdrop-filter: blur(15px);
  -webkit-backdrop-filter: blur(15px);
  border: 1px solid rgba(255, 255, 255, 0.15);
  border-radius: 10px;
  color: rgba(255, 255, 255, 0.95);
  font-size: 0.9rem;
  white-space: nowrap;
  pointer-events: none;
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.4);
}

.tooltip-arrow {
  position: absolute;
  top: -6px;
  left: 50%;
  transform: translateX(-50%) rotate(45deg);
  width: 12px;
  height: 12px;
  background: rgba(255, 255, 255, 0.05);
  border-left: 1px solid rgba(255, 255, 255, 0.15);
  border-top: 1px solid rgba(255, 255, 255, 0.15);
  z-index: -1;
  clip-path: polygon(0% 0%, 100% 0%, 0% 100%);
}

.tooltip-content {
  display: flex;
  align-items: center;
  gap: 10px;
  font-weight: 500;
}

.tooltip-content :deep(svg) {
  font-size: 1.2rem;
  color: #4facfe;
}

.tooltip-fade-enter-active,
.tooltip-fade-leave-active {
  transition: opacity 0.2s ease, transform 0.2s ease;
}

.tooltip-fade-enter-from,
.tooltip-fade-leave-to {
  opacity: 0;
  transform: translate(-50%, 5px); /* 保持水平居中的同时向下位移 */
}
</style>

