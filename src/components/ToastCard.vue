<template>
  <Transition name="toast">
    <div class="toast-card" v-if="show">
      <div class="toast-content">
        <Icon icon="ph:warning-bold" width="50" height="50" class="toast-icon" />
        <p style="color: white; font-weight: 500; font-size: 1.7rem;">{{ message }}</p>
      </div>
    </div>
  </Transition>
</template>

<script setup lang="js">
import { Icon } from '@iconify/vue';

defineProps({
  show: Boolean,
  message: { type: String, default: '请输入搜索内容后再试' }
});
</script>

<style scoped>
.toast-card {
  position: fixed;
  top: 40%; /* 出现在屏幕上方偏中位置 */
  left: 50%;
  transform: translateX(-50%);
  z-index: 10000;
  
  /* 玻璃拟态风格 */
  background: rgba(255, 69, 58, 0.1); /* 淡淡的警告红 */
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
  border: 1px solid rgba(255, 69, 58, 0.4);
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
  padding: 16px;
  border-radius: 12px;
  min-width: 280px;
}

.toast-content {
  display: flex;
  align-items: center;
  gap: 12px;
  color: white;
  font-weight: 500;
  letter-spacing: 1px;
}

.toast-icon {
  font-size: 1.2rem;
}

/* 动画：滑入并迅速滑出 */
.toast-enter-active {
  transition: all 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275);
}

.toast-leave-active {
  transition: all 0.3s cubic-bezier(0.4, 0, 1, 1);
}

.toast-enter-from {
  opacity: 0;
  transform: translate(-50%, 20px); /* 从下方滑入 */
}

.toast-leave-to {
  opacity: 0;
  transform: translate(-50%, -40px); /* 向上迅速消失 */
}
</style>
