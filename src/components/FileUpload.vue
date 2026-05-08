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
        <Icon icon="ph:upload-simple-bold" width="20" height="20" />
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
    default: '上传文件进行分析'
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

<style>
/* 提示框全局样式（因为被 Teleport 到了 body，所以放在非 scoped style 里） */
.glass-tooltip {
  position: fixed;
  z-index: 99999;
  padding: 10px 16px;
  
  /* 极致的毛玻璃太空风格 */
  background: rgba(255, 255, 255, 0.05);
  backdrop-filter: blur(15px);
  -webkit-backdrop-filter: blur(15px);
  border: 1px solid rgba(255, 255, 255, 0.15);
  border-radius: 10px;
  
  color: rgba(255, 255, 255, 0.95);
  font-size: 0.9rem;
  letter-spacing: 0.5px;
  pointer-events: none; /* 确保它不会遮挡鼠标对其他元素的点击 */
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.4);
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
