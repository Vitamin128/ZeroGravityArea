<template>
  <nav class="navbar">
    <div class="nav-container" ref="navContainer">
      <!-- 动态滑块 -->
      <div class="nav-indicator" :style="indicatorStyle"></div>

      <!-- 左侧链接 -->
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

      <!-- 中间 Logo -->
      <div class="nav-logo">
        <Icon icon="ph:planet-bold" class="logo-icon" />
        <span>Zero Gravity Area</span>
      </div>

      <!-- 右侧链接 -->
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

      <div class="nav-actions">
        <button class="icon-btn">
          <Icon icon="ph:user-circle-bold" />
        </button>
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

// 数据结构，方便管理
const leftLinks = [
  { name: '主页', path: '/search', icon: 'ph:magnifying-glass-bold' },
  { name: '搜索', path: '/history', icon: 'ph:magnifying-glass-bold' },
  { name: '伙伴', path: '/about', icon: 'tabler:alien' },
];

const rightLinks = [
  { name: '搜索', path: '/search1', icon: 'ph:magnifying-glass-bold' },
  { name: '历史', path: '/history1', icon: 'ph:clock-history-bold' },
  { name: '关于', path: '/about1', icon: 'ph:info-bold' },
];

// 动态存储每个 nav-item 的引用
const setItemRef = (el, path) => {
  if (el) itemRefs[path] = el.$el || el;
};

// 滑块样式状态
const indicatorStyle = reactive({
  width: '0px',
  left: '0px',
  opacity: 0
});

// 更新滑块位置的核心函数
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

// 监听路由变化或组件挂载，实时移动滑块
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
  position: relative; /* 为滑块提供定位基准 */
  display: flex;
  align-items: center;
  padding: 0 40px;
}

/* --- 动态滑块样式 --- */
.nav-indicator {
  position: absolute;
  height: 45px; /* 略小于导航栏高度，更精致 */
  background: rgba(79, 172, 254, 0.15); /* 淡蓝色发光背景 */
  border: 1px solid rgba(79, 172, 254, 0.3);
  border-radius: 12px;
  transition: all 0.4s cubic-bezier(0.23, 1, 0.32, 1); /* 使用贝塞尔曲线，让移动更有弹性感 */
  z-index: -1; /* 放在文字下面 */
}

.nav-links {
  display: flex;
  gap: 30px;
  margin: 0 40px;
}

.nav-item {
  text-decoration: none;
  color: rgba(255, 255, 255, 0.7);
  display: flex;
  align-items: center;
  gap: 8px;
  font-size: 1.1rem;
  padding: 8px 15px;
  border-radius: 12px;
  transition: color 0.3s;
  /* 移除原来的 background 样式，改用 indicator */
}

.router-link-active {
  color: #4facfe;
}

.nav-logo {
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
  font-size: 2.5rem;
  -webkit-text-fill-color: initial;
}

.icon-btn {
  background: none;
  border: none;
  color: white;
  font-size: 2rem;
  cursor: pointer;
  margin-left: auto;
}
</style>
