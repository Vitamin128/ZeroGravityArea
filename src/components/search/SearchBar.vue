<!-- src/components/SearchBar.vue -->
<template>
  <div class="search-box">
    <input 
      v-model="searchQuery" 
      type="text" 
      placeholder="探索星辰大海..." 
      @keyup.enter="callCppBackend" 
    />
    <button @click="callCppBackend" class="search-btn">
      <Icon icon="ph:magnifying-glass" width="18" height="18" />
    </button>
  </div>
</template>
<script setup lang="js">
import { ref, defineEmits } from 'vue';
import { Icon } from '@iconify/vue';
const emit = defineEmits(['update-results', 'search-start', 'empty-query']); // 新增 empty-query 事件
const searchQuery = ref('');
const callCppBackend = async () => {
  if (!searchQuery.value.trim()) {
    // 如果为空，发射空查询事件
    emit('empty-query');
    return;
  }
  // 1. 发射“开始搜索”信号，让父组件显示加载动画
  emit('search-start');
  try {
    const url = `http://192.168.52.131:8081/search?word=${encodeURIComponent(searchQuery.value)}`;
    const response = await fetch(url);
    const data = await response.json();
    
    // 2. 发射结果信号（父组件收到结果后会自动关闭加载）
    emit('update-results', data); 
    
  } catch (err) {
    console.error("RPC 调用失败:", err);
    emit('update-results', []); // 失败也传个空数组，触发关闭加载
  }
};
</script>

<style scoped>
/* 搜索框样式 */
.search-box {
  display: flex;
  background: #FFFFFF;
  border: 1px solid #E8E3DA;
  border-radius: 4px;
  padding: 6px 8px;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.08);
  transition: all 0.2s ease;
}

.search-box:focus-within {
  border-color: #8B6F47;
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.12);
}

input {
  flex: 1;
  background: transparent;
  border: none;
  outline: none;
  padding: 9px 18px;
  font-size: 1rem;
  color: #2C2C2C;
  width: 100%;
  font-family: 'Lora', serif;
}

input::placeholder {
  color: #B8B8B8;
}

.search-btn {
  background: #8B6F47;
  border: none;
  border-radius: 4px;
  width: 36px;
  height: 36px;
  cursor: pointer;
  display: flex;
  justify-content: center;
  align-items: center;
  transition: all 0.2s ease;
  color: white;
}

.search-btn:hover {
  background: #6B5437;
}
</style>
