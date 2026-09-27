-- Run this in Supabase SQL Editor.
create table if not exists public.notes (
  id uuid primary key default gen_random_uuid(),
  user_id uuid not null references auth.users(id) on delete cascade,
  title text not null check (char_length(title) between 1 and 80),
  content text not null,
  is_locked boolean not null default false,
  note_password text,
  deleted_at timestamptz,
  created_at timestamptz not null default now(),
  updated_at timestamptz not null default now()
);

create index if not exists notes_user_id_idx on public.notes(user_id);
create index if not exists notes_deleted_at_idx on public.notes(user_id, deleted_at);

alter table public.notes enable row level security;

drop policy if exists "Users can view their own notes" on public.notes;
create policy "Users can view their own notes" on public.notes for select using (auth.uid() = user_id);

drop policy if exists "Users can create their own notes" on public.notes;
create policy "Users can create their own notes" on public.notes for insert with check (auth.uid() = user_id);

drop policy if exists "Users can update their own notes" on public.notes;
create policy "Users can update their own notes" on public.notes for update using (auth.uid() = user_id) with check (auth.uid() = user_id);

drop policy if exists "Users can delete their own notes" on public.notes;
create policy "Users can delete their own notes" on public.notes for delete using (auth.uid() = user_id);

create or replace function public.set_notes_updated_at()
returns trigger language plpgsql as $$
begin
  new.updated_at = now();
  return new;
end;
$$;

drop trigger if exists notes_updated_at on public.notes;
create trigger notes_updated_at before update on public.notes for each row execute function public.set_notes_updated_at();

-- Username-to-email mapping for login with either username or email.
create table if not exists public.profiles (
  id uuid primary key references auth.users(id) on delete cascade,
  username text unique not null,
  email text unique not null
);

alter table public.profiles enable row level security;

create or replace function public.create_profile_for_user()
returns trigger language plpgsql security definer set search_path = public as $$
begin
  insert into public.profiles (id, username, email)
  values (new.id, coalesce(new.raw_user_meta_data->>'username', split_part(new.email, '@', 1)), new.email)
  on conflict (id) do update set username = excluded.username, email = excluded.email;
  return new;
end;
$$;

drop trigger if exists auth_user_profile on auth.users;
create trigger auth_user_profile after insert or update on auth.users for each row execute function public.create_profile_for_user();

insert into public.profiles (id, username, email)
select id, coalesce(raw_user_meta_data->>'username', split_part(email, '@', 1)), email
from auth.users
on conflict (id) do update set username = excluded.username, email = excluded.email;

create or replace function public.get_email_for_username(lookup_username text)
returns text language sql security definer set search_path = public as $$
  select email from public.profiles where lower(username) = lower(lookup_username) limit 1;
$$;

revoke all on function public.get_email_for_username(text) from public;
grant execute on function public.get_email_for_username(text) to anon, authenticated;
