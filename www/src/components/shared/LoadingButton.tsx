import { Button } from './Button'

interface Props {
  onClick: () => void
  loading: boolean
  loadingText: string
  text: string
  disabled?: boolean
  variant?: 'primary' | 'secondary'
  className?: string
}

export function LoadingButton({ 
  onClick, 
  loading, 
  loadingText, 
  text,
  disabled,
  variant,
  className
}: Props) {
  return (
    <Button
      onClick={onClick}
      disabled={loading || disabled}
      variant={variant}
      className={className}
    >
      {loading ? loadingText : text}
    </Button>
  )
}
