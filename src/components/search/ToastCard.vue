<template>
  <Transition name="toast">
    <div class="toast-card" v-if="show">
      <div class="toast-content">
        <Icon icon="ph:warning" width="24" height="24" class="toast-icon" />
        <p>{{ message }}</p>
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
  top: 40%;
  left: 50%;
  transform: translateX(-50%);
  z-index: 10000;
  background: #FFFFFF;
  border: 1px solid #E8E3DA;
  border-left: 3px solid #D32F2F;
  box-shadow: 0 4px 16px rgba(0, 0, 0, 0.12);
  padding: 16px 20px;
  border-radius: 4px;
  min-width: 280px;
}

.toast-content {
  display: flex;
  align-items: center;
  gap: 12px;
  color: #2C2C2C;
  font-weight: 500;
  font-family: 'Inter', sans-serif;
  font-size: 0.95rem;
}

.toast-content p {
  margin: 0;
}

.toast-icon {
  color: #D32F2F;
  flex-shrink: 0;
}

/* 动画 */
.toast-enter-active {
  transition: all 0.3s ease;
}

.toast-leave-active {
  transition: all 0.2s ease;
}

.toast-enter-from {
  opacity: 0;
  transform: translate(-50%, 20px);
}

.toast-leave-to {
  opacity: 0;
  transform: translate(-50%, -20px);
}
</style>
