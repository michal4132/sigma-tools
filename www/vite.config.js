import { defineConfig } from 'vite'
import preact from '@preact/preset-vite'
import { resolve } from 'path'

export default defineConfig({
  plugins: [preact()],
  server: {
    port: 3000
  },
  build: {
    outDir: resolve('../alt_app/www'),
    emptyOutDir: true
  }
})
