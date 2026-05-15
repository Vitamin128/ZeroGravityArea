<template>
  <!-- Toast 弹窗通知 -->
  <Transition name="toast">
    <div v-if="feedbackMsg" :class="['auth-toast', feedbackType]">
      <div class="auth-toast-content">
        <Icon 
          :icon="feedbackType === 'success' ? 'ph:check-circle-bold' : 'ph:warning-bold'" 
          width="24" height="24" class="auth-toast-icon" 
        />
        <p>{{ feedbackMsg }}</p>
      </div>
    </div>
  </Transition>

  <div class="auth-modal-overlay" @click.self="$emit('close')">
    <div class="auth-card-wrapper">
      <!-- 关闭按钮 -->
      <button class="close-btn" @click="$emit('close')">
        <Icon icon="ph:x-bold" />
      </button>

      <transition name="fade-slide" mode="out-in">
        <component 
          :is="currentViewComponent" 
          @switch="switchView" 
          @success="onAuthSuccess"
          @error="onAuthError"
        />
      </transition>
    </div>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue';
import { Icon } from '@iconify/vue';
import { verifyToken } from '@/api/auth';
import LoginForm from './LoginForm.vue';
import RegisterForm from './RegisterForm.vue';
import ForgotForm from './ForgotForm.vue';

const emit = defineEmits(['close']);
const currentView = ref('login');
const feedbackMsg = ref('');
const feedbackType = ref('error'); // 'error' | 'success'

const currentViewComponent = computed(() => {
  const map = {
    login: LoginForm,
    register: RegisterForm,
    forgot: ForgotForm
  };
  return map[currentView.value];
});

const showFeedback = (msg, type = 'error') => {
  feedbackMsg.value = msg;
  feedbackType.value = type;
  setTimeout(() => { feedbackMsg.value = ''; }, 3000);
};

const switchView = (view) => {
  currentView.value = view;
  feedbackMsg.value = '';
};

const onAuthSuccess = (msg) => {
  showFeedback(msg, 'success');
  if (currentView.value === 'login') {
    setTimeout(() => emit('close'), 800);
  }
};

const onAuthError = (msg) => {
  showFeedback(msg, 'error');
};

onMounted(async () => {
  const token = localStorage.getItem('auth_token');
  if (!token) return;

  try {
    const data = await verifyToken(token);
    const userInfo = data.data;
    if (userInfo) {
      if (userInfo.nickname) localStorage.setItem('nickname', userInfo.nickname);
      if (userInfo.avatar_url) localStorage.setItem('avatar_url', userInfo.avatar_url);
      if (userInfo.user_id) localStorage.setItem('user_id', userInfo.user_id);
      if (userInfo.email) localStorage.setItem('user_email', userInfo.email);
    }
    showFeedback('已自动登录', 'success');
    setTimeout(() => emit('close'), 800);
  } catch (err) {
    console.error('Token 校验失败:', err);
    localStorage.removeItem('auth_token');
  }
});
</script>

<style scoped>
/* 只保留外壳和 Toast 样式 */
.auth-modal-overlay {
  position: fixed;
  top: 0; left: 0;
  width: 100vw; height: 100vh;
  background: transparent;
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 2000;
}

.auth-card-wrapper {
  position: relative;
  width: 400px;
  background: rgba(255, 255, 255, 0.05);
  backdrop-filter: blur(40px);
  -webkit-backdrop-filter: blur(40px);
  border: 1px solid rgba(255, 255, 255, 0.15);
  border-top: 1px solid rgba(255, 255, 255, 0.25);
  border-radius: 24px;
  padding: 50px 40px;
  box-shadow: 0 10px 40px 0 rgba(0, 0, 0, 0.5), inset 0 1px 0 0 rgba(255, 255, 255, 0.1);
  color: white;
  overflow: hidden;
  animation: authFadeIn 0.8s ease-out;
}

@keyframes authFadeIn {
  from { opacity: 0; transform: translateY(20px); }
  to { opacity: 1; transform: translateY(0); }
}

.close-btn {
  position: absolute;
  top: 20px; right: 20px;
  background: none; border: none;
  color: rgba(255, 255, 255, 0.5);
  font-size: 1.5rem;
  cursor: pointer;
  transition: all 0.3s ease;
  z-index: 10;
}

.close-btn:hover {
  color: white;
  transform: rotate(90deg);
}

/* Toast 样式 */
.auth-toast {
  position: fixed; top: 20%; left: 50%; transform: translateX(-50%);
  z-index: 10000; backdrop-filter: blur(12px); padding: 16px; border-radius: 12px;
}
.auth-toast.error { background: rgba(255, 69, 58, 0.1); border: 1px solid rgba(255, 69, 58, 0.4); }
.auth-toast.success { background: rgba(0, 242, 254, 0.1); border: 1px solid rgba(0, 242, 254, 0.4); }
.auth-toast-content { display: flex; align-items: center; gap: 12px; color: white; font-size: 1.3rem; }

/* 切换动画 */
.fade-slide-enter-active, .fade-slide-leave-active { transition: all 0.3s ease; }
.fade-slide-enter-from { opacity: 0; transform: translateX(20px); }
.fade-slide-leave-to { opacity: 0; transform: translateX(-20px); }

.toast-enter-active { transition: all 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275); }
.toast-leave-active { transition: all 0.3s ease; }
.toast-enter-from { opacity: 0; transform: translate(-50%, 20px); }
.toast-leave-to { opacity: 0; transform: translate(-50%, -40px); }
</style>
