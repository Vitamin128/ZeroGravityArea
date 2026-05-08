<template>
  <div class="upload-btn-wrapper">
    <button class="upload-label" :title="title" @click="$emit('upload')">
      <slot>
        <!-- 默认上传图标 -->
        <Icon icon="ph:upload-simple-bold" width="20" height="20" />
      </slot>
    </button>
  </div>
</template>

<script setup>
import { Icon } from '@iconify/vue';

defineProps({
  title: {
    type: String,
    default: '上传文件进行分析'
  }
});

defineEmits(['upload']);
</script>

<style scoped>
.upload-btn-wrapper {
  /* 由父组件决定定位方式，保持组件的通用性 */
  display: inline-block;
}

.upload-label {
  display: flex;
  align-items: center;
  justify-content: center;
  width: 44px;
  height: 44px;
  background: rgba(255, 255, 255, 0.05);
  backdrop-filter: blur(10px);
  border: 1px solid rgba(255, 255, 255, 0.2);
  border-radius: 12px;
  color: #4facfe;
  cursor: pointer;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
}

.upload-label:hover {
  background: rgba(79, 172, 254, 0.15);
  border-color: #4facfe;
  box-shadow: 0 0 15px rgba(79, 172, 254, 0.3);
  transform: scale(1.05);
}

.upload-label:active {
  transform: scale(0.95);
}
</style>
