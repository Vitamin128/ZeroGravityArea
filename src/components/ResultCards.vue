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
  {{ item.title }}{{ item.type === 0 ? '.html' : (item.type === 1 ? '.pdf' : '') }}
</h2>
          <p class="card-content">{{ item.content }}</p>
        </div>
        <button class="download-btn" @click.stop="downloadItem(item)">
          <svg viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg">
            <path d="M12 16L7 11H10V4H14V11H17L12 16ZM5 18H19V20H5V18Z" />
          </svg>
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

const props = defineProps({
  results: { type: Array, default: () => [] }
});

const pageSize = 4;

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
  
  if (!docId) {
    console.error('错误：当前卡片数据中没有 doc_id', item);
    return;
  }
  // 2. 拼接 URL。建议地址与你搜索接口的域名保持一致（比如都是 127.0.0.1）
  const downloadUrl = `http://127.0.0.1:8081/download?doc_id=${docId}`;
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
/* P5 风格卡片核心逻辑 */

.card-entrance {
  animation: fadeInUp 0.6s ease-out both;
  width: 100%;
}
.cards-container {
  display: flex;
  flex-direction: column;
  gap: 16px;
  width: 100%;
  /* max-width: 720px; */
  margin: 0 auto;
  /* padding: 0 20px; */
}

/* 卡片：玻璃拟态样式 */
.result-card {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 20px;
  background: rgba(255, 255, 255, 0.05);
  backdrop-filter: blur(15px);
  -webkit-backdrop-filter: blur(15px);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 16px;
  padding: 24px 28px;
  cursor: pointer;
  transition: all 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275);
}

.card-body {
  flex: 9;          /* 占据 90% 的空间 */
  text-align: left;
  min-width: 0;     /* 关键：允许内容缩小，防止挤开按钮 */
}

.download-btn {
  flex: 1;          /* 占据 10% 的空间 */
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 8px;
  padding: 10px 18px;
  background: rgba(255, 255, 255, 0.08);
  border: 1px solid rgba(255, 255, 255, 0.12);
  border-radius: 12px;
  color: #ffffff;
  cursor: pointer;
  transition: all 0.3s ease;
  backdrop-filter: blur(10px);
  font-family: inherit;
  font-weight: 500;
  white-space: nowrap;
  min-width: fit-content;
}

.download-btn:hover {
  background: linear-gradient(135deg, #00aeec 0%, #00f2fe 100%);
  border-color: transparent;
  color: #0f3460;
  transform: translateY(-2px);
  box-shadow: 0 8px 20px rgba(0, 174, 236, 0.4);
}

.download-btn svg {
  width: 20px;
  height: 20px;
  fill: currentColor;
}

.result-card:hover {
  transform: translateY(-5px) scale(1.02);
  
  /* 2. 背景透明度对齐搜索框 */
  background: rgba(255, 255, 255, 0.1);
  
  /* 3. 边框颜色对齐搜索框（使用半透明白） */
  border-color: rgba(255, 255, 255, 0.3);
  
  /* 4. 阴影强度对齐搜索框 */
  box-shadow: 0 20px 40px rgba(0, 0, 0, 0.4);
}

/* 卡片头部 */
.card-header {
  display: flex;
  align-items: center;
  gap: 14px;
  margin-bottom: 14px;
}

.card-index {
  font-size: 1.2rem;
  font-weight: 700;
  color: #0f3460;
  background: linear-gradient(135deg, #4facfe 0%, #00f2fe 100%);
  padding: 4px 10px;
  border-radius: 8px;
  letter-spacing: 1px;
  flex-shrink: 0;
}

.card-title {
  font-size: 1.2rem;
  font-weight: 600;
  margin: 0;
  color: #ffffff;
  letter-spacing: -0.3px;
}

/* 卡片正文 */
.card-content {
  font-size: 1.2rem;
  line-height: 1.7;
  color: rgba(255, 255, 255, 0.55);
  margin: 0 0 16px 0;
  display: -webkit-box;
  -webkit-line-clamp: 2;
  -webkit-box-orient: vertical;
  line-clamp: 2;
  overflow: hidden;
}

/* 卡片底部 */
.card-footer {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding-top: 14px;
  border-top: 1px solid rgba(255, 255, 255, 0.06);
}

.card-path {
  font-size: 0.75rem;
  color: rgba(255, 255, 255, 0.3);
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
  max-width: 70%;
  font-family: 'Courier New', monospace;
}

.card-weight {
  font-size: 0.75rem;
  font-weight: 600;
  color: rgba(79, 172, 254, 0.7);
  flex-shrink: 0;
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
.pagination-wrapper {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 12px;
  margin-top: 40px;
  padding: 20px 0;
  animation: fadeIn 0.8s ease-out;
}
.page-numbers {
  display: flex;
  gap: 8px;
}
/* 按钮通用基础样式：玻璃拟态 */
.page-btn, .page-num {
  background: rgba(255, 255, 255, 0.05);
  backdrop-filter: blur(10px);
  -webkit-backdrop-filter: blur(10px);
  border: 1px solid rgba(255, 255, 255, 0.1);
  color: white;
  padding: 8px 8px;
  border-radius: 8px;
  cursor: pointer;
  transition: all 0.3s ease;
  font-family: inherit;
  font-size: 1.3rem;
}
.page-num {
  padding: 8px 8px;
  min-width: 40px;
}
/* 悬停效果 */
.page-btn:hover:not(:disabled), .page-num:hover:not(.dot) {
  background: rgba(255, 255, 255, 0.15);
  border-color: rgba(255, 255, 255, 0.3);
  transform: translateY(-2px);
}
/* 激活状态：B 站蓝渐变 */
.page-num.active {
  background: linear-gradient(135deg, #00aeec 0%, #00f2fe 100%);
  border: none;
  color: #0f3460;
  font-weight: 700;
  box-shadow: 0 4px 15px rgba(0, 174, 236, 0.4);
}
/* 禁用状态 */
.page-btn:disabled {
  opacity: 0.3;
  cursor: not-allowed;
}
/* 省略号样式 */
.page-num.dot {
  background: transparent;
  border: none;
  cursor: default;
}
@keyframes fadeIn {
  from { opacity: 0; }
  to { opacity: 1; }
}
</style>
