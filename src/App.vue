<template>
  <!-- 背景保留在这里 -->
  <div class="app-container">
    <div class="search-wrapper">
      <h1 class="title">Zero Gravity Search</h1>

      <!-- 使用引入的搜索框组件 -->
      <SearchBar />
    </div>
    <!-- 搜索结果卡片组件 -->
    <div class="cards-wrapper">
      <ResultCards />
    </div>
  </div>
</template>
<script setup lang="js">
// 引入组件
import SearchBar from './components/SearchBar.vue';
import ResultCards from './components/ResultCards.vue';
</script>
<style scoped>
/* 容器背景和布局样式保留 */
.app-container {
  display: flex;
  flex-direction: column;
  align-items: center;
  min-height: 100vh;
  background: linear-gradient(135deg, #1a1a2e 0%, #16213e 50%, #0f3460 100%);
  font-family: 'Outfit', sans-serif;
  color: white;
  padding-bottom: 60px;
}
.search-wrapper {
  text-align: center;
  width: 35%;
  padding: 20px;
  margin-top: 20px;
  margin-bottom: 30px;
}
.cards-wrapper {
  width: 35%;
  padding: 20px;
}
.title {
  font-size: 2.5rem;
  margin-bottom: 2rem;
  background: linear-gradient(to right, #4facfe 0%, #00f2fe 100%);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}
</style>