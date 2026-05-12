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
          <button class="icon-btn">
            <Icon icon="ph:user-circle-bold" />
          </button>
        </div>
      </div>

    </div>
  </nav>
</template>

<script setup lang="js">
import { ref, onMounted, watch, reactive } from 'vue';
import { useRoute } from 'vue-router';
import { Icon } from '@iconify/vue';

const route = useRoute();
const navContainer = ref(null);
const itemRefs = reactive({});

const leftLinks = [
  { name: 'Game', path: '/search', icon: 'ion:game-controller-outline' },
  { name: '搜索', path: '/history', icon: 'ph:magnifying-glass-bold' },
  { name: '伙伴', path: '/about', icon: 'tabler:alien' },
];

const rightLinks = [
  { name: '消息', path: '/search1', icon: 'tabler:message' },
  { name: '书馆', path: '/history1', icon: 'akar-icons:book' },
  { name: '个人主页', path: '/about1', icon: 'material-symbols:home-outline-rounded' },
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
  background: linear-gradient(to right, #4facfe, #00f2fe);
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
  font-size: 2rem;
  cursor: pointer;
  display: flex;
  align-items: center;
}

.nav-item :deep(svg) {
  font-size: 1.6rem; /* 你可以根据感觉调整这个数字，比如 1.4rem 或 1.8rem */
  transition: transform 0.3s ease; /* 让图标的大小变化也带点平滑感 */
}
/* 悬停时让图标再稍微变大一点（可选，效果很赞） */
.nav-item:hover :deep(svg) {
  transform: scale(1.1); 
}
</style>
