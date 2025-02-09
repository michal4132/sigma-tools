/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  darkMode: 'class',
  theme: {
    extend: {
      colors: {
        'bg-primary': '#0F172A',
        'bg-secondary': '#1E293B',
        'bg-accent': '#334155',
        'primary': '#38BDF8',
        'secondary': '#818CF8',
        'success': '#34D399',
        'warning': '#FBBF24',
        'danger': '#FB7185',
        'text-primary': '#F1F5F9',
        'text-secondary': '#94A3B8',
        'text-muted': '#64748B'
      }
    },
  },
  plugins: [
    function ({ addBase, theme }) {
      addBase({
        'html': {
          'box-sizing': 'border-box'
        }
      })
    },
    require('@tailwindcss/forms'),
    require('tailwind-scrollbar')({ nocompatible: true })
  ],
}
