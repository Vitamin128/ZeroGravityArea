<template>
  <div class="cards-container">
    <!-- 1. 只渲染当前页的数据 -->
    <div 
      class="card-entrance" 
      v-for="(item, index) in paginatedResults" 
      :key="index"
    >
      <div class="result-card">
        <div class="card-body">
          <h2 class="card-title">
  {{ item.title }}
</h2>
          <p class="card-content">{{ item.content }}</p>
        </div>
        <button class="download-btn" @click.stop="downloadItem(item)">
          <Icon icon="ph:download-simple" width="20" height="20" />
          <span>下载</span>
        </button>
      </div>
    </div>
    <!-- 2. 翻页器：只有总页数 > 1 时才显示 -->
    <div class="pagination-wrapper" v-if="totalPages > 1">
      <button 
        class="page-btn prev" 
        :disabled="currentPage === 1" 
        @click="changePage(currentPage - 1)"
      >上一页</button>
      <div class="page-numbers">
        <button 
          v-for="page in visiblePages" 
          :key="page"
          class="page-num"
          :class="{ active: currentPage === page, dot: page === '...' }"
          @click="page !== '...' && changePage(page)"
        >
          {{ page }}
        </button>
      </div>
      <button 
        class="page-btn next" 
        :disabled="currentPage === totalPages" 
        @click="changePage(currentPage + 1)"
      >下一页</button>
    </div>
  </div>
</template>
<script setup lang="js">
import { ref, computed, defineProps, watch } from 'vue';
import { Icon } from '@iconify/vue';

const props = defineProps({
  results: { type: Array, default: () => [] }
});

const pageSize = 12;

const currentPage = ref(1);
// 当搜索结果改变时，自动重置回第一页


watch(() => props.results, () => {
  currentPage.value = 1;
});

const totalPages = computed(() => Math.ceil(props.results.length / pageSize));

// 当前页显示的数据
const paginatedResults = computed(() => {
  const start = (currentPage.value - 1) * pageSize;
  const end = start + pageSize;
  console.log(props.results.slice(start, end));
  return props.results.slice(start, end);
});

const changePage = (page) => {
  currentPage.value = page;
  // 翻页后平滑滚动到顶部（可选）
  window.scrollTo({ top: 0, behavior: 'smooth' });
};

// 仿 B 站的页码显示逻辑 [1, ..., 4, 5, 6, ..., 28]
const visiblePages = computed(() => {
  const total = totalPages.value;
  const current = currentPage.value;
  const pages = [];
  if (total <= 7) {
    for (let i = 1; i <= total; i++) pages.push(i);
  } else {
    pages.push(1);
    if (current > 4) pages.push('...');
    
    let start = Math.max(2, current - 2);
    let end = Math.min(total - 1, current + 2);
    
    if (current <= 4) end = 5;
    if (current > total - 4) start = total - 4;
    for (let i = start; i <= end; i++) pages.push(i);
    
    if (current < total - 3) pages.push('...');
    pages.push(total);
  }
  return pages;
});

const downloadItem = (item) => {
  // 1. 使用正确的字段名 doc_id
  const docId = item.doc_id; 
  
  if (docId === undefined || docId === null || docId === '') {
    console.error('错误：当前卡片数据中没有 doc_id', item);
    return;
  }
  // 2. 拼接 URL。建议地址与你搜索接口的域名保持一致（比如都是 124.220.21.204）
  const downloadUrl = `http://192.168.52.131:8081/download?doc_id=${docId}`;
  console.log('正在请求下载:', downloadUrl);
  // 3. 触发下载
  const link = document.createElement('a');
  link.href = downloadUrl;
  
  // 这一步很重要：防止某些浏览器因跨域安全限制阻断跳转
  link.target = '_blank'; 
  
  document.body.appendChild(link);
  link.click();
  document.body.removeChild(link);
};
</script>

<style scoped>
/* 卡片容器 */
.card-entrance {
  animation: fadeInUp 0.6s ease-out both;
  width: 30%;
}

.cards-container {
  display: flex;
  flex-wrap: wrap;
  justify-content: flex-start;
  gap: 20px 1%;
  width: 100%;
  padding-left: 4%;
  margin: 0 auto;
}

/* 卡片样式 */
.result-card {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 2px;
  height: 130px;
  box-sizing: border-box;
  background: #FFFFFF;
  border: 1px solid #E8E3DA;
  border-radius: 4px;
  padding: 24px 20px 15px 20px;
  cursor: pointer;
  transition: all 0.2s ease;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.08);
}

.card-body {
  flex: 9;
  text-align: left;
  min-width: 0;
  height: 100%;
  overflow-y: auto;
  padding-bottom: 24px;
  scrollbar-width: thin;
  scrollbar-color: #D4C4B0 #F7F4ED;
}

.card-body::-webkit-scrollbar {
  width: 6px;
}

.card-body::-webkit-scrollbar-track {
  background: #F7F4ED;
}

.card-body::-webkit-scrollbar-thumb {
  background: #D4C4B0;
  border-radius: 3px;
}

.card-body::-webkit-scrollbar-thumb:hover {
  background: #8B6F47;
}

.download-btn {
  flex: 1;
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 8px;
  padding: 8px 14px;
  background: #8B6F47;
  border: none;
  border-radius: 4px;
  color: #ffffff;
  cursor: pointer;
  transition: all 0.2s ease;
  font-family: 'Inter', sans-serif;
  font-weight: 500;
  white-space: nowrap;
  min-width: fit-content;
}

.download-btn:hover {
  background: #6B5437;
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(139, 111, 71, 0.3);
}

.download-btn :deep(svg) {
  width: 20px;
  height: 20px;
  fill: currentColor;
}

.result-card:hover {
  transform: translateY(-2px);
  border-color: #8B6F47;
  box-shadow: 0 4px 16px rgba(0, 0, 0, 0.12);
}

.card-title {
  font-size: 1.1rem;
  font-weight: 600;
  margin: 0;
  color: #2C2C2C;
  letter-spacing: -0.3px;
  font-family: 'Inter', sans-serif;
}

.card-content {
  font-size: 0.95rem;
  line-height: 1.7;
  color: #6B6B6B;
  margin: 0 0 16px 0;
  font-family: 'Lora', serif;
}

/* 入场动画 */
@keyframes fadeInUp {
  from {
    opacity: 0;
    transform: translateY(30px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

/* 分页器 */
.pagination-wrapper {
  width: 100%;
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 12px;
  margin-top: 40px;
  padding: 20px 0;
  padding-right: 8%;
  animation: fadeIn 0.8s ease-out;
}

.page-numbers {
  display: flex;
  gap: 8px;
}

.page-btn, .page-num {
  background: #FFFFFF;
  border: 1px solid #E8E3DA;
  color: #2C2C2C;
  padding: 6px 6px;
  border-radius: 4px;
  cursor: pointer;
  transition: all 0.2s ease;
  font-family: 'Inter', sans-serif;
  font-size: 1rem;
}

.page-num {
  padding: 6px 6px;
  min-width: 30px;
}

.page-btn:hover:not(:disabled), .page-num:hover:not(.dot) {
  background: #F7F4ED;
  border-color: #8B6F47;
}

.page-num.active {
  background: #8B6F47;
  border: none;
  color: #FFFFFF;
  font-weight: 600;
  box-shadow: 0 2px 8px rgba(139, 111, 71, 0.3);
}

.page-btn:disabled {
  opacity: 0.3;
  cursor: not-allowed;
}

.page-num.dot {
  background: transparent;
  border: none;
  cursor: default;
  color: #6B6B6B;
}

@keyframes fadeIn {
  from { opacity: 0; }
  to { opacity: 1; }
}
</style>
