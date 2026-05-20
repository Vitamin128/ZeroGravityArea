<template>
  <div class="page-container">
    <div class="construction-card">
      <div class="icon-wrapper">
        <Icon icon="svg-spinners:clock" width="64" height="64" />
      </div>
      <h2>此页面正在建设中</h2>
      <p>我们正在努力为您构建更棒的体验，敬请期待！</p>
      
      <div class="back-btn" @click="$router.push('/search')">
        返回首页
      </div>
    </div>
  </div>
</template>

<script setup lang="js">
import { Icon } from '@iconify/vue';
</script>

<style scoped>
.page-container {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 80vh;
  padding: 20px;
}

.construction-card {
  background: rgba(255, 255, 255, 0.03);
  backdrop-filter: blur(15px);
  -webkit-backdrop-filter: blur(15px);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 24px;
  padding: 60px 40px;
  text-align: center;
  max-width: 450px;
  width: 100%;
  box-shadow: 0 8px 32px 0 rgba(0, 0, 0, 0.37);
  animation: fadeIn 0.8s ease-out;
}

.icon-wrapper {
  color: #4facfe;
  margin-bottom: 24px;
  display: flex;
  justify-content: center;
}

h2 {
  color: white;
  font-size: 1.8rem;
  margin-bottom: 12px;
  font-weight: 700;
  letter-spacing: 1px;
}

p {
  color: rgba(255, 255, 255, 0.6);
  font-size: 1.1rem;
  line-height: 1.6;
  margin-bottom: 32px;
}

.back-btn {
  display: inline-block;
  padding: 12px 32px;
  background: rgba(79, 172, 254, 0.15);
  border: 1px solid rgba(79, 172, 254, 0.3);
  color: #4facfe;
  border-radius: 12px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s ease;
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
}

.back-btn:hover {
  background: rgba(79, 172, 254, 0.25);
  border-color: rgba(79, 172, 254, 0.5);
  transform: translateY(-2px);
  box-shadow: 0 6px 15px rgba(79, 172, 254, 0.2);
}

@keyframes fadeIn {
  from { opacity: 0; transform: translateY(20px); }
  to { opacity: 1; transform: translateY(0); }
}
</style>
