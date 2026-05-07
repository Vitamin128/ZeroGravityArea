import { createApp } from 'vue'
import App from './App.vue'
import Particles from "@tsparticles/vue3"
import { loadSlim } from "@tsparticles/slim"
const app = createApp(App)
// 注册 Particles 插件
app.use(Particles, {
    init: async engine => {
        // 这里的 loadSlim 会让你的粒子效果更轻量，适合大多数场景
        await loadSlim(engine);
    },
})
app.mount('#app')