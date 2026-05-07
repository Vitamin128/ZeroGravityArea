<template>
  <div class="app-container">
    <div class="search-wrapper">
      <h1 class="title">Zero Gravity Search</h1>
      <div class="search-box">
        <input 
          v-model="searchQuery" 
          type="text" 
          placeholder="探索星辰大海..." 
          @keyup.enter="handleSearch"
          id="search-input"
        />
        <button @click="handleSearch" class="search-btn" id="search-button">
          <span class="icon">🔍</span>
        </button>
      </div>
    </div>
  </div>
</template>

<script setup lang="js">
import { ref } from 'vue';

// 搜索关键词响应式变量
const searchQuery = ref('');

// 搜索处理函数
const handleSearch = () => {
  if (searchQuery.value.trim()) {
    // 默认跳转到百度搜索
    window.open(`https://www.baidu.com/s?wd=${encodeURIComponent(searchQuery.value)}`, '_blank');
  } else {
    alert('请输入搜索内容');
  }
};
</script>

<style scoped>
/* 全屏容器：使用高级渐变背景 */
.app-container {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  margin: 0;
  background: linear-gradient(135deg, #1a1a2e 0%, #16213e 50%, #0f3460 100%);
  font-family: 'Outfit', 'Inter', system-ui, -apple-system, sans-serif;
  color: white;
}

.search-wrapper {
  text-align: center;
  width: 100%;
  max-width: 600px;
  padding: 20px;
}

.title {
  font-size: 2.5rem;
  font-weight: 700;
  margin-bottom: 2rem;
  letter-spacing: -1px;
  background: linear-gradient(to right, #4facfe 0%, #00f2fe 100%);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  animation: fadeInDown 0.8s ease-out;
}

/* 搜索框：磨砂玻璃效果 */
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
  font-size: 1.1rem;
  color: white;
  width: 100%;
}

input::placeholder {
  color: rgba(255, 255, 255, 0.4);
}

/* 搜索按钮 */
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
  transition: transform 0.3s ease, box-shadow 0.3s ease;
}

.search-btn:hover {
  transform: rotate(15deg) scale(1.1);
  box-shadow: 0 0 20px rgba(79, 172, 254, 0.6);
}

.icon {
  font-size: 1.2rem;
}

/* 入场动画 */
@keyframes fadeInDown {
  from {
    opacity: 0;
    transform: translateY(-20px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}
</style>
