import { createApp } from 'vue'
import App from './App.vue'
import Particles from "@tsparticles/vue3"
import { loadStarsPreset } from "@tsparticles/preset-stars"
import router from './router'

const app = createApp(App)

app.use(router)
app.use(Particles, {
    init: async engine => {
        await loadStarsPreset(engine);
    },
})

app.mount('#app')