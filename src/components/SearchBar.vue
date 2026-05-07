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
      <span class="icon">🔍</span>
    </button>
  </div>
</template>
<script setup lang="js">
import { ref, defineEmits } from 'vue';
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
    const url = `http://127.0.0.1:8081/search?word=${encodeURIComponent(searchQuery.value)}`;
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
/* 搜索框专属样式（玻璃拟态） */
.search-box {
  display: flex;
  background: rgba(255, 255, 255, 0.05);
  backdrop-filter: blur(15px);
  -webkit-backdrop-filter: blur(15px);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 50px;
  padding: 8px 10px;
  box-shadow: 0 15px 35px rgba(0, 0, 0, 0.2);
  transition: all 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275);
}

.search-box:focus-within {
  transform: translateY(-5px) scale(1.02);
  background: rgba(255, 255, 255, 0.1);
  border-color: rgba(255, 255, 255, 0.3);
  box-shadow: 0 20px 40px rgba(0, 0, 0, 0.4);
}

input {
  flex: 1;
  background: transparent;
  border: none;
  outline: none;
  padding: 12px 25px;
  font-size: 1.5rem;
  color: white;
  width: 100%;
}

input::placeholder {
  color: rgba(255, 255, 255, 0.4);
}

.search-btn {
  background: linear-gradient(135deg, #00f2fe 0%, #4facfe 100%);
  border: none;
  border-radius: 50%;
  width: 48px;
  height: 48px;
  cursor: pointer;
  display: flex;
  justify-content: center;
  align-items: center;
  transition: transform 0.3s ease;
}

.search-btn:hover {
  transform: rotate(15deg) scale(1.1);
}

.icon {
  font-size: 1.2rem;
}
</style>
