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
  border: 3px solid #E8E3DA;
  border-top-color: #8B6F47;
  border-radius: 50%;
  animation: spin 0.8s linear infinite;
}

.loading-text {
  margin-top: 16px;
  color: #6B6B6B;
  font-size: 1rem;
  letter-spacing: 1px;
  text-align: left;
  font-family: 'Inter', sans-serif;
}

@keyframes spin {
  to { transform: rotate(360deg); }
}
</style>
