<template>
  <div class="upload-btn-wrapper">
    <!-- 移除原生的 title 属性，添加鼠标事件监听 -->
    <button 
      class="upload-label" 
      @click="$emit('upload')"
      @mouseenter="onMouseEnter"
      @mouseleave="onMouseLeave"
      @mousemove="onMouseMove"
    >
      <slot>
        <!-- 默认上传图标 -->
        <Icon icon="ph:upload-simple" width="20" height="20" />
      </slot>
    </button>

    <!-- 自定义悬浮毛玻璃提示框，挂载到 body 以避免层级和定位被裁剪 -->
    <Teleport to="body">
      <Transition name="tooltip-fade">
        <div 
          v-if="showTooltip" 
          class="glass-tooltip" 
          :style="{ left: mouseX + 'px', top: mouseY + 'px' }"
        >
          {{ title }}
        </div>
      </Transition>
    </Teleport>
  </div>
</template>

<script setup>
import { ref } from 'vue';
import { Icon } from '@iconify/vue';

defineProps({
  title: {
    type: String,
    default: '上传文件存储'
  }
});

defineEmits(['upload']);

const showTooltip = ref(false);
const mouseX = ref(0);
const mouseY = ref(0);

const onMouseEnter = (e) => {
  showTooltip.value = true;
  updatePosition(e);
};

const onMouseLeave = () => {
  showTooltip.value = false;
};

const onMouseMove = (e) => {
  updatePosition(e);
};

const updatePosition = (e) => {
  // 增加 15px 偏移量，让提示框跟随在鼠标右下方，避免挡住鼠标指针本身
  mouseX.value = e.clientX + 15;
  mouseY.value = e.clientY + 15;
};
</script>

<style scoped>
.upload-btn-wrapper {
  display: inline-block;
}

.upload-label {
  display: flex;
  align-items: center;
  justify-content: center;
  width: 44px;
  height: 44px;
  background: #FFFFFF;
  border: 1px solid #E8E3DA;
  border-radius: 4px;
  color: #8B6F47;
  cursor: pointer;
  transition: all 0.2s ease;
}

.upload-label:hover {
  background: #F7F4ED;
  border-color: #8B6F47;
  box-shadow: 0 2px 8px rgba(139, 111, 71, 0.2);
}

.upload-label:active {
  transform: scale(0.95);
}
</style>

<style>
/* 提示框全局样式 */
.glass-tooltip {
  position: fixed;
  z-index: 99999;
  padding: 10px 16px;
  background: #2C2C2C;
  border: 1px solid #E8E3DA;
  border-radius: 4px;
  color: #FFFFFF;
  font-size: 0.85rem;
  font-family: 'Inter', sans-serif;
  pointer-events: none;
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
}

/* 提示框淡入淡出动画 */
.tooltip-fade-enter-active,
.tooltip-fade-leave-active {
  transition: opacity 0.2s ease, transform 0.2s ease;
}

.tooltip-fade-enter-from,
.tooltip-fade-leave-to {
  opacity: 0;
  transform: translateY(5px);
}
</style>
