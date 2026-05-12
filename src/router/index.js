import { createRouter, createWebHistory } from 'vue-router'
import MainSearch from '../components/MainSearch.vue'

const routes = [
  {
    path: '/search',
    name: 'Home',
    component: MainSearch
  }
]

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes
})

export default router
