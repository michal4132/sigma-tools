interface ErrorAlertProps {
  error: string | null
  className?: string
}

export function ErrorAlert({ error, className = '' }: ErrorAlertProps) {
  if (!error) return null

  return (
    <div className={`p-4 bg-danger bg-opacity-10 text-danger rounded-lg ${className}`}>
      {error}
    </div>
  )
}
