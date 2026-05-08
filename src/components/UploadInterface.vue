<template>
  <Transition name="modal">
    <div class="modal-overlay" v-if="visible" @click.self="close">
      <div class="modal-container">
        <!-- ===== 头部区域 ===== -->
        <div class="modal-header">
          <h2 class="modal-title">附件</h2>
          <button class="close-btn" @click="close" title="关闭">
            <Icon icon="ph:x-bold" width="18" height="18" />
          </button>
        </div>
        <div class="header-divider"></div>

        <!-- ===== 核心上传区 ===== -->
        <div
          class="upload-zone"
          :class="{ 'drag-active': isDragOver }"
          @dragover.prevent="onDragOver"
          @dragleave.prevent="onDragLeave"
          @drop.prevent="onDrop"
          @click="triggerFileInput"
        >
          <!-- 状态 2：拖拽悬停态 —— 毛玻璃遮罩 -->
          <Transition name="blur-fade">
            <div class="drag-overlay" v-if="isDragOver">
              <div class="drag-overlay-content">
                <p class="drag-hint-title">在此处拖放文件</p>
                <div class="drag-hint-limits">
                  <span>文件数量：最多 50 个</span>
                  <span>文件类型：pdf, txt, csv, docx, doc, xlsx, xls, pptx, ppt, md, mobi, epub</span>
                </div>
              </div>
            </div>
          </Transition>

          <!-- 状态 1：默认态 —— 图标 + 提示文字（无文件时） -->
          <div class="upload-placeholder" v-if="files.length === 0 && !isDragOver">
            <Icon icon="ph:cloud-arrow-up" width="48" height="48" class="upload-icon" />
            <p class="upload-hint">拖拽即可添加附件</p>
          </div>

          <!-- 状态 3：文件列表态 —— 已添加的文件 -->
          <div class="file-list" v-if="files.length > 0 && !isDragOver">
            <div
              class="file-item"
              v-for="(file, index) in files"
              :key="index"
            >
              <!-- 左侧：文件类型图标 -->
              <Icon :icon="getFileIcon(file.name)" width="28" height="28" class="file-type-icon" />

              <!-- 中间：文件信息 -->
              <div class="file-info">
                <span class="file-name">{{ file.name }}</span>
                <span class="file-size">{{ formatFileSize(file.size) }}</span>
              </div>

              <!-- 右侧：删除按钮（悬停时显示） -->
              <button class="file-delete-btn" @click.stop="removeFile(index)" title="移除文件">
                <Icon icon="ph:trash-bold" width="16" height="16" />
              </button>
            </div>
          </div>
        </div>

        <!-- 隐藏的 file input -->
        <input
          ref="fileInputRef"
          type="file"
          multiple
          :accept="acceptTypes"
          @change="onFileInputChange"
          hidden
        />

        <!-- ===== 底部操作区 ===== -->
        <div class="modal-footer">
          <button class="confirm-btn" :disabled="files.length === 0 || isUploading" @click="handleConfirm">
            <Icon v-if="isUploading" icon="ph:spinner" width="18" height="18" class="spin-icon" />
            <span>{{ isUploading ? '上传中...' : '确定' }}</span>
          </button>
        </div>
      </div>
    </div>
  </Transition>
</template>

<script setup>
import { ref } from 'vue';
import { Icon } from '@iconify/vue';

defineProps({
  visible: {
    type: Boolean,
    default: false
  }
});

const emit = defineEmits(['close', 'upload-success']);

// 允许的文件类型
const allowedExtensions = ['pdf', 'txt', 'csv', 'docx', 'doc', 'xlsx', 'xls', 'pptx', 'ppt', 'md', 'mobi', 'epub'];
const acceptTypes = allowedExtensions.map(ext => `.${ext}`).join(',');
const MAX_FILES = 50;

// 响应式状态
const isDragOver = ref(false);
const files = ref([]);
const isUploading = ref(false);
const fileInputRef = ref(null);

// ===== 拖拽事件处理 =====
const onDragOver = () => {
  isDragOver.value = true;
};

const onDragLeave = () => {
  isDragOver.value = false;
};

const onDrop = (event) => {
  isDragOver.value = false;
  const droppedFiles = Array.from(event.dataTransfer.files);
  addFiles(droppedFiles);
};

// ===== 点击选择文件 =====
const triggerFileInput = () => {
  // 只在默认态（无文件）时触发点击选择
  if (files.value.length === 0) {
    fileInputRef.value?.click();
  }
};

const onFileInputChange = (event) => {
  const selectedFiles = Array.from(event.target.files);
  addFiles(selectedFiles);
  event.target.value = ''; // 重置以便再次选择同一文件
};

// ===== 文件管理 =====
const addFiles = (newFiles) => {
  // 过滤合法扩展名
  const validFiles = newFiles.filter(file => {
    const ext = file.name.split('.').pop().toLowerCase();
    return allowedExtensions.includes(ext);
  });

  // 限制总数不超过 50
  const remaining = MAX_FILES - files.value.length;
  const filesToAdd = validFiles.slice(0, remaining);

  files.value.push(...filesToAdd);
};

const removeFile = (index) => {
  files.value.splice(index, 1);
};

// ===== 关闭弹窗 =====
const close = () => {
  emit('close');
};

// ===== 格式化文件大小 =====
const formatFileSize = (bytes) => {
  if (bytes < 1024) return bytes + ' B';
  if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(2) + ' K';
  return (bytes / (1024 * 1024)).toFixed(2) + ' M';
};

// ===== 根据文件扩展名返回合适的图标 =====
const getFileIcon = (filename) => {
  const ext = filename.split('.').pop().toLowerCase();
  const iconMap = {
    pdf: 'ph:file-pdf-bold',
    txt: 'ph:file-text-bold',
    csv: 'ph:file-csv-bold',
    md: 'ph:file-text-bold',
    doc: 'ph:file-doc-bold',
    docx: 'ph:file-doc-bold',
    xls: 'ph:file-xls-bold',
    xlsx: 'ph:file-xls-bold',
    ppt: 'ph:file-ppt-bold',
    pptx: 'ph:file-ppt-bold',
    mobi: 'ph:book-bold',
    epub: 'ph:book-bold'
  };
  return iconMap[ext] || 'ph:file-bold';
};

// ===== Mock 上传 =====
const handleConfirm = async () => {
  if (files.value.length === 0 || isUploading.value) return;

  isUploading.value = true;
  console.log('开始上传以下文件:');
  files.value.forEach((f, i) => {
    console.log(`  [${i + 1}] ${f.name}  (${formatFileSize(f.size)})`);
  });

  // 模拟异步上传（1.5 秒延迟）
  await new Promise(resolve => setTimeout(resolve, 1500));

  console.log('✅ 所有文件上传成功！');
  isUploading.value = false;
  emit('upload-success', [...files.value]);
  files.value = [];
  close();
};
</script>

<style scoped>
/* ========== 模态框遮罩层 ========== */
.modal-overlay {
  position: fixed;
  inset: 0;
  z-index: 9000;
  display: flex;
  align-items: center;
  justify-content: center;
  background: rgba(0, 0, 0, 0.5);
  backdrop-filter: blur(4px);
}

/* ========== 模态框容器 ========== */
.modal-container {
  width: 1040px;
  max-height: 85vh;
  display: flex;
  flex-direction: column;
  background: rgba(22, 33, 62, 0.95);
  backdrop-filter: blur(20px);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 16px;
  box-shadow: 0 25px 60px rgba(0, 0, 0, 0.5);
  overflow: hidden;
}

/* ========== 头部区域 ========== */
.modal-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 18px 24px;
}

.modal-title {
  margin: 0;
  font-size: 1.1rem;
  font-weight: 600;
  color: rgba(255, 255, 255, 0.9);
  letter-spacing: 0.5px;
}

.close-btn {
  display: flex;
  align-items: center;
  justify-content: center;
  width: 32px;
  height: 32px;
  background: transparent;
  border: none;
  border-radius: 8px;
  color: rgba(255, 255, 255, 0.5);
  cursor: pointer;
  transition: all 0.25s ease;
}

.close-btn:hover {
  background: rgba(255, 255, 255, 0.1);
  color: rgba(255, 255, 255, 0.9);
}

/* 极细的浅色分割线 */
.header-divider {
  height: 1px;
  background: rgba(255, 255, 255, 0.08);
  margin: 0 24px;
}

/* ========== 核心上传区 ========== */
.upload-zone {
  position: relative;
  margin: 20px 24px;
  min-height: 520px;
  border: 2px dashed rgba(255, 255, 255, 0.15);
  border-radius: 12px;
  display: flex;
  align-items: center;
  justify-content: center;
  transition: all 0.3s ease;
  cursor: pointer;
  overflow: hidden;
}

/* 拖拽激活时边框高亮 */
.upload-zone.drag-active {
  border-color: rgba(79, 172, 254, 0.5);
  cursor: default;
}

/* ----- 状态 1：默认态 ----- */
.upload-placeholder {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 16px;
  pointer-events: none;
}

.upload-icon {
  color: rgba(79, 172, 254, 0.5);
}

.upload-hint {
  margin: 0;
  font-size: 0.95rem;
  color: rgba(255, 255, 255, 0.35);
  letter-spacing: 0.5px;
}

/* ----- 状态 2：拖拽悬停态 —— 毛玻璃遮罩 ----- */
.drag-overlay {
  position: absolute;
  inset: 0;
  z-index: 10;
  display: flex;
  align-items: center;
  justify-content: center;
  background: rgba(15, 52, 96, 0.3);
  backdrop-filter: blur(6px);
  border-radius: 10px;
}

.drag-overlay-content {
  text-align: center;
  z-index: 11; /* 确保文本在模糊层之上，保持清晰 */
}

.drag-hint-title {
  margin: 0 0 12px 0;
  font-size: 1.15rem;
  font-weight: 600;
  color: #ffffff;
  text-shadow: 0 2px 8px rgba(0, 0, 0, 0.4);
}

.drag-hint-limits {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.drag-hint-limits span {
  font-size: 0.78rem;
  color: rgba(255, 255, 255, 0.65);
  text-shadow: 0 1px 4px rgba(0, 0, 0, 0.3);
}

/* ----- 状态 3：文件列表态 ----- */
.file-list {
  width: 100%;
  max-height: 300px;
  overflow-y: auto;
  padding: 8px 4px;
}

/* 自定义滚动条 */
.file-list::-webkit-scrollbar {
  width: 4px;
}

.file-list::-webkit-scrollbar-thumb {
  background: rgba(255, 255, 255, 0.15);
  border-radius: 2px;
}

/* 单条文件项 */
.file-item {
  display: flex;
  align-items: center;
  gap: 12px;
  padding: 10px 14px;
  border-radius: 8px;
  transition: background 0.2s ease;
}

.file-item:hover {
  background: rgba(255, 255, 255, 0.05);
}

.file-type-icon {
  color: rgba(79, 172, 254, 0.7);
  flex-shrink: 0;
}

.file-info {
  display: flex;
  flex-direction: column;
  gap: 2px;
  flex: 1;
  min-width: 0;
}

.file-name {
  font-size: 0.88rem;
  color: rgba(255, 255, 255, 0.85);
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}

.file-size {
  font-size: 0.75rem;
  color: rgba(255, 255, 255, 0.35);
}

/* 删除按钮：默认隐藏，悬停时显示 */
.file-delete-btn {
  display: flex;
  align-items: center;
  justify-content: center;
  width: 28px;
  height: 28px;
  background: transparent;
  border: none;
  border-radius: 6px;
  color: rgba(255, 255, 255, 0.3);
  cursor: pointer;
  opacity: 0;
  transition: all 0.2s ease;
  flex-shrink: 0;
}

.file-item:hover .file-delete-btn {
  opacity: 1;
}

.file-delete-btn:hover {
  background: rgba(255, 69, 58, 0.15);
  color: #ff453a;
}

/* ========== 底部操作区 ========== */
.modal-footer {
  display: flex;
  justify-content: flex-end;
  padding: 16px 24px;
}

.confirm-btn {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 10px 28px;
  background: linear-gradient(135deg, #00aeec 0%, #4facfe 100%);
  border: none;
  border-radius: 10px;
  color: white;
  font-size: 0.9rem;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  letter-spacing: 0.5px;
}

.confirm-btn:hover:not(:disabled) {
  transform: translateY(-2px);
  box-shadow: 0 8px 20px rgba(0, 174, 236, 0.35);
}

.confirm-btn:active:not(:disabled) {
  transform: translateY(0);
}

.confirm-btn:disabled {
  opacity: 0.4;
  cursor: not-allowed;
}

/* 加载旋转动画 */
.spin-icon {
  animation: spin 1s linear infinite;
}

@keyframes spin {
  from { transform: rotate(0deg); }
  to { transform: rotate(360deg); }
}

/* ========== 模态框出入动画 ========== */
.modal-enter-active {
  transition: opacity 0.3s ease;
}

.modal-enter-active .modal-container {
  transition: transform 0.3s cubic-bezier(0.175, 0.885, 0.32, 1.275), opacity 0.3s ease;
}

.modal-leave-active {
  transition: opacity 0.2s ease;
}

.modal-leave-active .modal-container {
  transition: transform 0.2s ease, opacity 0.2s ease;
}

.modal-enter-from {
  opacity: 0;
}

.modal-enter-from .modal-container {
  transform: scale(0.9) translateY(20px);
  opacity: 0;
}

.modal-leave-to {
  opacity: 0;
}

.modal-leave-to .modal-container {
  transform: scale(0.95) translateY(10px);
  opacity: 0;
}

/* 拖拽遮罩淡入淡出 */
.blur-fade-enter-active {
  transition: opacity 0.25s ease;
}

.blur-fade-leave-active {
  transition: opacity 0.15s ease;
}

.blur-fade-enter-from,
.blur-fade-leave-to {
  opacity: 0;
}
</style>
