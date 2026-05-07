<template>
  <div class="app-container">
    <div class="search-wrapper">
      <h1 class="title">Zero Gravity Search</h1>
      <div class="search-box">
        <input 
          v-model="searchQuery" 
          type="text" 
          placeholder="搜索你想要的..." 
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
const searchQuery = ref('');
const handleSearch = () => {
  if (searchQuery.value.trim()) {
    window.open(`https://www.baidu.com/s?wd=${encodeURIComponent(searchQuery.value)}`, '_blank');
  } else {
    alert('请输入搜索内容');
  }
};
</script>
<style scoped>
/* 使用用户提供的清新渐变色 */
.app-container {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  margin: 0;
  background-image: linear-gradient(120deg, #84fab0 0%, #8fd3f4 100%);
  font-family: 'Outfit', 'Inter', system-ui, -apple-system, sans-serif;
  color: #2c3e50; /* 切换为深色文字以适配浅色背景 */
}
.search-wrapper {
  text-align: center;
  width: 100%;
  max-width: 600px;
  padding: 20px;
}
.title {
  font-size: 2.8rem;
  font-weight: 800;
  margin-bottom: 2rem;
  letter-spacing: -1.5px;
  /* 标题颜色调整为深色渐变，增加层次感 */
  background: linear-gradient(to right, #243949 0%, #517fa4 100%);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  animation: fadeInDown 0.8s ease-out;
}
/* 亮色玻璃拟态效果 */
.search-box {
  display: flex;
  background: rgba(255, 255, 255, 0.4); /* 增加白色透明度 */
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
  border: 1px solid rgba(255, 255, 255, 0.6);
  border-radius: 50px;
  padding: 8px 10px;
  /* 阴影改为柔和的深色，突出浮空感 */
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.08);
  transition: all 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275);
}
.search-box:focus-within {
  transform: translateY(-5px);
  background: rgba(255, 255, 255, 0.6);
  box-shadow: 0 15px 40px rgba(0, 0, 0, 0.12);
}
input {
  flex: 1;
  background: transparent;
  border: none;
  outline: none;
  padding: 12px 25px;
  font-size: 1.1rem;
  color: #2c3e50; /* 输入文字颜色 */
  width: 100%;
}
input::placeholder {
  color: rgba(44, 62, 80, 0.5);
}
/* 按钮颜色也同步调整为更协调的深蓝色系 */
.search-btn {
  background: linear-gradient(135deg, #2c3e50 0%, #4b6cb7 100%);
  border: none;
  border-radius: 50%;
  width: 48px;
  height: 48px;
  cursor: pointer;
  display: flex;
  justify-content: center;
  align-items: center;
  transition: all 0.3s ease;
  box-shadow: 0 4px 15px rgba(44, 62, 80, 0.2);
}
.search-btn:hover {
  transform: scale(1.1);
  box-shadow: 0 6px 20px rgba(44, 62, 80, 0.3);
}
.icon {
  font-size: 1.2rem;
  filter: brightness(0) invert(1); /* 将黑色搜索图标变为白色 */
}
@keyframes fadeInDown {
  from { opacity: 0; transform: translateY(-20px); }
  to { opacity: 1; transform: translateY(0); }
}
</style>