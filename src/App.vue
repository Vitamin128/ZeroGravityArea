<template>
  <div class="app-container">
    <vue-particles id="tsparticles" :options="particlesOptions" />
    
    <ToastCard :show="showToast" />
    <div class="search-wrapper">
      <h1 class="title">Zero Gravity Search</h1>
      
      <!-- 搜索区域锚点 -->
      <div class="search-anchor">
        <SearchBar 
          @search-start="handleStart" 
          @update-results="handleResults" 
          @empty-query="triggerToast" 
        />
        
        <!-- 文件上传按钮 (已组件化) -->
        <div class="upload-pos-wrapper">
          <FileUpload @upload="openUploadModal" />
        </div>
      </div>
    </div>

    <!-- 文件上传模态框 -->
    <UploadInterface
      :visible="showUploadModal"
      @close="showUploadModal = false"
      @upload-success="onUploadSuccess"
    />
    
    <!-- 加载中：显示旋转动画 -->
    <LoadingOverlay :active="isLoading" />

    <!-- 加载完成且有结果：显示卡片 -->
    <div class="cards-wrapper" v-if="!isLoading && searchResults.length > 0">
      <ResultCards :results="searchResults" />
    </div>
  </div>
</template>
<script setup lang="js">
import { ref } from 'vue';
import ToastCard from './components/ToastCard.vue';
import SearchBar from './components/SearchBar.vue';
import ResultCards from './components/ResultCards.vue';
import FileUpload from './components/FileUpload.vue';
import UploadInterface from './components/UploadInterface.vue';
// 引入自定义加载组件
import LoadingOverlay from './components/LoadingOverlay.vue';

const showToast = ref(false);
const triggerToast = () => {
  if (showToast.value) return; // 防止重复触发
  showToast.value = true;
  
  // 1.8秒后自动关闭
  setTimeout(() => {
    showToast.value = false;
  }, 1800);
};


const searchResults = ref([]);
const isLoading = ref(false); // 【调试用】临时改为 true，确认组件是否正常渲染

const handleStart = () => {
  isLoading.value = true;    // 开启加载
  searchResults.value = [];  // 搜索新内容前清空旧结果，确保卡片隐藏
};

const handleResults = (data) => {
  searchResults.value = data; // 存入结果
  // 为了让用户看清加载动画（防止后端返回太快），可以加个小延迟
  setTimeout(() => {
    isLoading.value = false;  // 关闭加载
  }, 500);
};

// 文件上传模态框
const showUploadModal = ref(false);

const openUploadModal = () => {
  showUploadModal.value = true;
};

const onUploadSuccess = (uploadedFiles) => {
  console.log('上传成功，共', uploadedFiles.length, '个文件');
};


// 粒子背景：使用真实的星空预设
const particlesOptions = {
  preset: "stars",
  background: {
    color: { value: "transparent" } // 保持我们原本的渐变底色
  },
  particles: {
    number: {
      value: 150, // 稍微增加星星数量，显得更深邃
      density: { enable: true, value_area: 800 }
    },
    move: {
      speed: 0.8 // 放慢星星移动速度，增加失重感
    }
  }
};
</script>
<!-- 全局样式：背景放在 body 上，不能用 scoped（否则会影响 fixed 定位） -->
<style>
body {
  margin: 0;
  min-height: 100vh;
  background: linear-gradient(135deg, #1a1a2e 0%, #16213e 50%, #0f3460 100%);
  font-family: 'Outfit', sans-serif;
  color: white;
  overflow-x: hidden;
}
</style>

<style scoped>
/* app-container 不再设置 background/overflow，避免创建堆叠上下文限制 fixed 定位 */
.app-container {
  position: relative;
  display: flex;
  flex-direction: column;
  align-items: center;
  min-height: 100vh;
}
/* 粒子背景：改用 fixed，随视口定位 */
#tsparticles {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  z-index: 0;
  pointer-events: none; /* 让鼠标事件穿透到下层内容 */
}
/* 确保内容在粒子层之上 */
.search-wrapper, .cards-wrapper {
  position: relative;
  z-index: 1;
}
.search-wrapper {
  text-align: center;
  width: 35%;
  padding: 20px;
  margin-top: 20px;
  margin-bottom: 30px;
}
.cards-wrapper {
  width: 100%;
  padding: 20px;
}

/* 搜索框锚点：确保按钮定位基准 */
.search-anchor {
  position: relative;
  width: 100%;
}

/* 上传按钮定位包裹器：仅负责绝对定位到右侧 */
.upload-pos-wrapper {
  position: absolute;
  left: calc(100% + 15px);
  top: 50%;
  transform: translateY(-50%);
}
.title {
  font-size: 2.0rem;
  margin-bottom: 2rem;
  background: linear-gradient(to right, #4facfe 0%, #00f2fe 100%);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}
</style>