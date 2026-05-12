<template>
  <div class="app-container">
    <ToastCard :show="showToast" />
    <div class="search-wrapper">
      <!-- <h1 class="title">Zero Gravity Area</h1> -->
      
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
// 路径更新：从 components 移出到 views，所以需要回退一层到 components
import ToastCard from '../components/ToastCard.vue';
import SearchBar from '../components/SearchBar.vue';
import ResultCards from '../components/ResultCards.vue';
import FileUpload from '../components/FileUpload.vue';
import UploadInterface from '../components/UploadInterface.vue';
import LoadingOverlay from '../components/LoadingOverlay.vue';

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
const isLoading = ref(false);

const handleStart = () => {
  isLoading.value = true;    // 开启加载
  searchResults.value = [];  // 搜索新内容前清空旧结果
};

const handleResults = (data) => {
  searchResults.value = data; // 存入结果
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
</script>

<style scoped>
.app-container {
  position: relative;
  display: flex;
  flex-direction: column;
  align-items: center;
  /* 移除 min-height: 100vh，交给 App.vue 控制 */
  width: 100%;
}
.search-wrapper, .cards-wrapper {
  position: relative;
  z-index: 1;
}
.search-wrapper {
  text-align: center;
  width: 30%;
  padding: 15px;
  margin-top: 50px;
  margin-bottom: 22px;
}
.cards-wrapper {
  width: 100%;
  padding: 20px;
}
.search-anchor {
  position: relative;
  width: 100%;
}
.upload-pos-wrapper {
  position: absolute;
  left: calc(100% + 11px);
  top: 50%;
  transform: translateY(-50%);
}
.title {
  font-size: 1.8rem;
  margin-bottom: 1.5rem;
  background: linear-gradient(to right, #4facfe 0%, #00f2fe 100%);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}
</style>
