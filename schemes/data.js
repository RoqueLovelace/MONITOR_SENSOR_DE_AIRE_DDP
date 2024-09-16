import z from 'zod';

const dataScheme = z.object({
  ppm: z.number({
    invalid_type_error: 'ppm must be a number',
    required_error: 'ppm is required'
  }).int({
    invalid_type_error: 'ppm must be an integer'
  }).min(0),
  date: z.string({
    invalid_type_error: 'date must be a string',
    required_error: 'date is required'
  }).date(),
  time: z.string({
    invalid_type_error: 'time must be a string',
    required_error: 'time is required'
  }).time()
});

export function ValidateParse (object) {
  return dataScheme.safeParse(object);
}
