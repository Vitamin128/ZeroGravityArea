<template>
  <div class="loading-inline" v-if="active">
    <div class="spinner"></div>
    <!-- 动态显示 dots 变量 -->
    <p class="loading-text">搜索中{{ dots }}</p>
  </div>
</template>

<script setup lang="js">
import { ref, onMounted, onUnmounted, watch } from 'vue';

const props = defineProps({
  active: { type: Boolean, default: false }
});

const dots = ref('');
let timer = null;

// 开始动画的函数
const startAnimation = () => {
  if (timer) return;
  timer = setInterval(() => {
    if (dots.value.length >= 3) {
      dots.value = '';
    } else {
      dots.value += '.';
    }
  }, 500);
};

// 停止动画的函数
const stopAnimation = () => {
  if (timer) {
    clearInterval(timer);
    timer = null;
    dots.value = '';
  }
};

// 监听 active 属性，只有在加载时才跑定时器
watch(() => props.active, (newVal) => {
  if (newVal) {
    startAnimation();
  } else {
    stopAnimation();
  }
}, { immediate: true });

// 组件卸载时安全清除定时器
onUnmounted(() => {
  stopAnimation();
});
</script>

<style scoped>
/* 样式保持你之前的修改 */
.loading-inline {
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  padding: 45px 0;
}

.spinner {
  width: 38px;
  height: 38px;
  border: 3px solid rgba(79, 172, 254, 0.15);
  border-top-color: #4facfe;
  border-radius: 50%;
  animation: spin 0.8s linear infinite;
}

.loading-text {
  margin-top: 16px;
  color: rgba(255, 255, 255, 0.5);
  font-size: 1.5rem;
  letter-spacing: 2px;
  /* 固定宽度防止文字跳动 */
  /* min-width: 120px; */
  text-align: left;
}

@keyframes spin {
  to { transform: rotate(360deg); }
}
</style>
