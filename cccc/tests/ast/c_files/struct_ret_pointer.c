struct Movie
{
    char* name;
    int times_watched;
};

struct Movie new_movie(char* name)
{
    struct Movie mov;
    mov.name = name;
    mov.times_watched = 0;
    return mov;
}

struct Movie* watch(struct Movie* mov)
{
    mov->times_watched = (*mov).times_watched + 1;
    return mov;
}

int main()
{
    struct Movie film;
    (film = new_movie("Dinge die keiner wissen will"));
    struct Movie (*film_referenz);
    (film_referenz = watch(&film));

}