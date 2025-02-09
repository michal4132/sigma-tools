import { ComponentChildren } from 'preact'

interface ButtonProps {
  children: ComponentChildren
  onClick?: () => void
  disabled?: boolean
  className?: string
  type?: 'button' | 'submit' | 'reset'
  variant?: 'primary' | 'secondary'
}

export function Button({ 
  children, 
  onClick, 
  disabled = false, 
  className = '',
  type = 'button',
  variant = 'primary'
}: ButtonProps) {
  const baseStyles = 'px-4 py-2 rounded-md transition-colors whitespace-nowrap disabled:opacity-50 disabled:cursor-not-allowed'
  const variantStyles = variant === 'primary' 
    ? 'bg-primary text-text-primary hover:bg-opacity-90'
    : 'bg-bg-accent text-text-primary hover:bg-opacity-90'

  return (
    <button
      type={type}
      onClick={onClick}
      disabled={disabled}
      className={`${baseStyles} ${variantStyles} ${className}`}
    >
      {children}
    </button>
  )
}
