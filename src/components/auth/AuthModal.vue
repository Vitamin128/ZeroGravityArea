<template>
  <!-- Toast 弹窗通知 -->
  <Transition name="toast">
    <div v-if="feedbackMsg" :class="['auth-toast', feedbackType]">
      <Icon
        :icon="feedbackType === 'success' ? 'ph:check-circle' : 'ph:warning-circle'"
        width="20" height="20" class="toast-icon"
      />
      <p>{{ feedbackMsg }}</p>
    </div>
  </Transition>

  <div class="auth-modal-overlay" @click.self="$emit('close')">
    <div class="auth-card">
      <!-- 关闭按钮 -->
      <button class="close-btn" @click="$emit('close')">
        <Icon icon="ph:x" />
      </button>

      <transition name="fade" mode="out-in">
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
import SettingsForm from './SettingsForm.vue';

const props = defineProps({
  initialView: {
    type: String,
    default: 'login'
  }
});

const emit = defineEmits(['close']);
const currentView = ref(props.initialView);
const feedbackMsg = ref('');
const feedbackType = ref('error'); // 'error' | 'success'

const currentViewComponent = computed(() => {
  const map = {
    login: LoginForm,
    register: RegisterForm,
    forgot: ForgotForm,
    settings: SettingsForm
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
  if (currentView.value === 'login' || currentView.value === 'settings') {
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

    if (currentView.value !== 'settings') {
      showFeedback('已自动登录', 'success');
      setTimeout(() => emit('close'), 800);
    }
  } catch (err) {
    console.error('Token 校验失败:', err);
    localStorage.removeItem('auth_token');
  }
});
</script>

<style scoped>
/* 模态框遮罩 */
.auth-modal-overlay {
  position: fixed;
  top: 0;
  left: 0;
  width: 100vw;
  height: 100vh;
  background: rgba(0, 0, 0, 0.4);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 2000;
  animation: overlayFadeIn 0.3s ease;
}

@keyframes overlayFadeIn {
  from { opacity: 0; }
  to { opacity: 1; }
}

/* 认证卡片 */
.auth-card {
  position: relative;
  width: 420px;
  max-width: 90vw;
  background: #FFFFFF;
  border: 1px solid #E8E3DA;
  border-radius: 4px;
  padding: 48px 40px;
  box-shadow: 0 4px 16px rgba(0, 0, 0, 0.12);
  animation: cardSlideIn 0.4s ease;
}

@keyframes cardSlideIn {
  from {
    opacity: 0;
    transform: translateY(20px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

/* 关闭按钮 */
.close-btn {
  position: absolute;
  top: 16px;
  right: 16px;
  background: none;
  border: none;
  color: #6B6B6B;
  font-size: 1.3rem;
  padding: 6px;
  display: flex;
  align-items: center;
  justify-content: center;
  border-radius: 2px;
  transition: all 0.2s ease;
  z-index: 10;
}

.close-btn:hover {
  color: #2C2C2C;
  background: #F7F4ED;
}

/* Toast 通知 */
.auth-toast {
  position: fixed;
  top: 80px;
  left: 50%;
  transform: translateX(-50%);
  z-index: 10000;
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 12px 20px;
  background: #FFFFFF;
  border-radius: 4px;
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.15);
  font-family: 'Inter', sans-serif;
  font-size: 0.9rem;
}

.auth-toast.error {
  border-left: 3px solid #D32F2F;
  color: #D32F2F;
}

.auth-toast.success {
  border-left: 3px solid #8B6F47;
  color: #8B6F47;
}

.auth-toast p {
  margin: 0;
  color: #2C2C2C;
}

.toast-icon {
  flex-shrink: 0;
}

/* 切换动画 */
.fade-enter-active,
.fade-leave-active {
  transition: opacity 0.2s ease;
}

.fade-enter-from,
.fade-leave-to {
  opacity: 0;
}

/* Toast 动画 */
.toast-enter-active {
  transition: all 0.3s ease;
}

.toast-leave-active {
  transition: all 0.2s ease;
}

.toast-enter-from {
  opacity: 0;
  transform: translate(-50%, -10px);
}

.toast-leave-to {
  opacity: 0;
  transform: translate(-50%, -10px);
}
</style>
