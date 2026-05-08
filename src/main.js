import { createApp } from 'vue'
import App from './App.vue'
import Particles from "@tsparticles/vue3"
import { loadStarsPreset } from "@tsparticles/preset-stars"

const app = createApp(App)

app.use(Particles, {
    init: async engine => {
        await loadStarsPreset(engine);
    },
})

app.mount('#app')