<template>
  <div class="search-container">
    <div class="search-box" :class="{ 'is-focused': isFocused }">
      <!-- <Icon icon="ph:magnifying-glass" width="20" height="20" class="prefix-icon" /> -->

      <input
        v-model="searchQuery"
        type="text"
        placeholder="输入关键词搜索文档内容..."
        @keyup.enter="callCppBackend"
        @focus="isFocused = true"
        @blur="isFocused = false"
      />

      <button
        v-if="searchQuery.trim()"
        @click="clearSearch"
        class="clear-btn"
        title="清空"
      >
        <Icon icon="ph:x" width="16" height="16" />
      </button>

      <!-- <span v-if="!searchQuery.trim()" class="shortcut-hint">Enter</span> -->

      <button
        @click="callCppBackend"
        class="search-btn"
        :disabled="isLoading"
      >
        <Icon
          v-if="isLoading"
          icon="ph:spinner"
          width="18"
          height="18"
          class="spin-icon"
        />
        <Icon
          v-else
          icon="ph:magnifying-glass"
          width="18"
          height="18"
        />
        <!-- <span>搜索</span> -->
      </button>
    </div>
  </div>
</template>

<script setup lang="js">
import { ref, defineEmits } from 'vue';
import { Icon } from '@iconify/vue';

const emit = defineEmits(['update-results', 'search-start', 'empty-query']);

const searchQuery = ref('');
const isFocused = ref(false);
const isLoading = ref(false);

const clearSearch = () => {
  searchQuery.value = '';
};

const callCppBackend = async () => {
  if (!searchQuery.value.trim()) {
    emit('empty-query');
    return;
  }

  isLoading.value = true;
  emit('search-start');

  try {
    const url = `http://192.168.52.131:8081/search?word=${encodeURIComponent(searchQuery.value)}`;
    const response = await fetch(url);
    const data = await response.json();
    emit('update-results', data);
  } catch (err) {
    console.error("RPC 调用失败:", err);
    emit('update-results', []);
  } finally {
    isLoading.value = false;
  }
};
</script>

<style scoped>
.search-container {
  display: flex;
  justify-content: center;
  align-items: center;
  width: 100%;
  padding: 40px 20px;
}

.search-box {
  display: flex;
  align-items: center;
  gap: 12px;
  width: 100%;
  max-width: 720px;
  background: #FFFFFF;
  border: 2px solid #E8E3DA;
  border-radius: 12px;
  padding: 6px 6px;
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.08);
  transition: all 0.3s ease;
}

.search-box.is-focused {
  border-color: #8B6F47;
  box-shadow: 0 4px 20px rgba(139, 111, 71, 0.2);
  transform: translateY(-2px);
}

.prefix-icon {
  color: #8B6F47;
  flex-shrink: 0;
}

input {
  flex: 1;
  background: transparent;
  border: none;
  outline: none;
  padding: 8px 12px;
  font-size: 1.05rem;
  color: #2C2C2C;
  font-family: 'Lora', serif;
  min-width: 0;
}

input::placeholder {
  color: #B8B8B8;
}

.clear-btn {
  display: flex;
  align-items: center;
  justify-content: center;
  width: 28px;
  height: 28px;
  background: transparent;
  border: none;
  border-radius: 50%;
  color: #6B6B6B;
  cursor: pointer;
  transition: all 0.2s ease;
  flex-shrink: 0;
}

.clear-btn:hover {
  background: #F7F4ED;
  color: #2C2C2C;
}

.shortcut-hint {
  padding: 4px 10px;
  background: #F7F4ED;
  border: 1px solid #E8E3DA;
  border-radius: 6px;
  color: #6B6B6B;
  font-size: 0.8rem;
  font-family: 'Inter', sans-serif;
  font-weight: 500;
  flex-shrink: 0;
}

.search-btn {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 10px 10px;
  background: transparent;
  border: 2px solid #8B6F47;
  border-radius: 8px;
  color: #8B6F47;
  font-size: 0.95rem;
  font-weight: 600;
  font-family: 'Inter', sans-serif;
  cursor: pointer;
  transition: all 0.2s ease;
  flex-shrink: 0;
  white-space: nowrap;
}

.search-btn:hover:not(:disabled) {
  background: #8B6F47;
  color: #FFFFFF;
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(139, 111, 71, 0.3);
}

.search-btn:active:not(:disabled) {
  transform: translateY(0);
}

.search-btn:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

.spin-icon {
  animation: spin 1s linear infinite;
}

@keyframes spin {
  from {
    transform: rotate(0deg);
  }
  to {
    transform: rotate(360deg);
  }
}
</style>
