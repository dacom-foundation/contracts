// @ts-check
import antfu from '@antfu/eslint-config'

export default antfu(
  {
    ignores: [
      // eslint ignore globs here
    ],
  },
  {
    rules: {
      'no-unused-vars': 'warn',
      'unused-imports/no-unused-vars': 'warn',
      'no-console': 'off',
      'unicorn/prefer-number-properties': 'off',
      // overrides
    },
  },
)
