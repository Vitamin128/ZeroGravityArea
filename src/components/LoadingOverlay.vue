<template>
  <div class="loading-inline" v-if="active">
    <div class="spinner"></div>
    <p class="loading-text">搜索中...</p>
  </div>
</template>

<script setup lang="js">
defineProps({
  active: { type: Boolean, default: false }
});
</script>

<style scoped>
.loading-inline {
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  padding: 60px 0;
}

.spinner {
  width: 50px;
  height: 50px;
  border: 3px solid rgba(79, 172, 254, 0.15);
  border-top-color: #4facfe;
  border-radius: 50%;
  animation: spin 0.8s linear infinite;
}

.loading-text {
  margin-top: 16px;
  color: rgba(255, 255, 255, 0.5);
  font-size: 1.5rem;
  letter-spacing: 2px;
}

@keyframes spin {
  to { transform: rotate(360deg); }
}
</style>
