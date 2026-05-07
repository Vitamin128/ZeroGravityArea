<template>
  <div class="cards-container">
    <div 
      class="result-card" 
      v-for="(item, index) in results" 
      :key="index"
      :style="{ animationDelay: index * 0.12 + 's' }"
    >
      <div class="card-header">
        <span class="card-index">{{ String(index + 1).padStart(2, '0') }}</span>
        <h2 class="card-title">{{ item.title }}</h2>
      </div>
      <p class="card-content">{{ item.content }}</p>
      <div class="card-footer">
        <span class="card-path">{{ item.local_path }}</span>
        <span class="card-weight">权重: {{ item.weight }}</span>
      </div>
    </div>
  </div>
</template>

<script setup lang="js">
import { ref } from 'vue';

const results = ref([
  {
    content: "...urely optional. One alternative is to define static factory functions for your classes. The factory function can create an object, pass ownership of...",
    local_path: "/home/bamboo/ZeroGravityArea/data/input/signals2/tutorial.html",
    title: "Tutorial",
    type: 0,
    weight: 1
  },
  {
    content: "...uded in Boost. If these indexes are not enough for you, you can define your own index type. To know how to do this, go to Building custom inde...",
    local_path: "/home/bamboo/ZeroGravityArea/data/input/interprocess/managed_memory_segments.html",
    title: "Managed Memory Segments",
    type: 0,
    weight: 1
  },
  {
    content: "...use the smart_library. The first thing to do when creating your own plugins is define the plugin interface. There is an example of a...",
    local_path: "/home/bamboo/ZeroGravityArea/data/input/boost_dll/mangled_import.html",
    title: "Mangled Import",
    type: 0,
    weight: 1
  },
  {
    content: "...d by the committee. By the way, it's a great opportunity to make your operator>> more general. Read a good C+...",
    local_path: "/home/bamboo/ZeroGravityArea/data/input/boost_lexical_cast/frequently_asked_questions.html",
    title: "Frequently Asked Questions",
    type: 0,
    weight: 1
  }
]);
</script>

<style scoped>
.cards-container {
  display: flex;
  flex-direction: column;
  gap: 16px;
  width: 100%;
  max-width: 720px;
  margin: 0 auto;
  padding: 0 20px;
}

/* 卡片：玻璃拟态样式 */
.result-card {
  background: rgba(255, 255, 255, 0.05);
  backdrop-filter: blur(15px);
  -webkit-backdrop-filter: blur(15px);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 16px;
  padding: 24px 28px;
  cursor: pointer;
  transition: all 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275);
  animation: fadeInUp 0.6s ease-out both;
}

.result-card:hover {
  transform: translateY(-6px) scale(1.015);
  background: rgba(255, 255, 255, 0.1);
  border-color: rgba(79, 172, 254, 0.4);
  box-shadow: 0 20px 40px rgba(0, 0, 0, 0.3),
              0 0 30px rgba(79, 172, 254, 0.08);
}

/* 卡片头部 */
.card-header {
  display: flex;
  align-items: center;
  gap: 14px;
  margin-bottom: 14px;
}

.card-index {
  font-size: 0.75rem;
  font-weight: 700;
  color: #0f3460;
  background: linear-gradient(135deg, #4facfe 0%, #00f2fe 100%);
  padding: 4px 10px;
  border-radius: 8px;
  letter-spacing: 1px;
  flex-shrink: 0;
}

.card-title {
  font-size: 1.15rem;
  font-weight: 600;
  margin: 0;
  color: #ffffff;
  letter-spacing: -0.3px;
}

/* 卡片正文 */
.card-content {
  font-size: 0.9rem;
  line-height: 1.7;
  color: rgba(255, 255, 255, 0.55);
  margin: 0 0 16px 0;
  display: -webkit-box;
  -webkit-line-clamp: 2;
  -webkit-box-orient: vertical;
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
</style>
