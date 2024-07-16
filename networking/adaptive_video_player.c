#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_QUALITY_LEVELS 5
#define BUFFER_SIZE 1024

typedef struct {
    int bitrate;
    char *url;
} QualityLevel;

typedef struct {
    QualityLevel levels[MAX_QUALITY_LEVELS];
    int num_levels;
    int current_level;
    int buffer_health;
} AdaptivePlayer;

void init_player(AdaptivePlayer *player) {
    player->num_levels = 0;
    player->current_level = 0;
    player->buffer_health = BUFFER_SIZE / 2;
}

void add_quality_level(AdaptivePlayer *player, int bitrate, const char *url) {
    if (player->num_levels < MAX_QUALITY_LEVELS) {
        player->levels[player->num_levels].bitrate = bitrate;
        player->levels[player->num_levels].url = strdup(url);
        player->num_levels++;
    }
}

void adapt_quality(AdaptivePlayer *player) {
    if (player->buffer_health < BUFFER_SIZE / 4 && player->current_level > 0) {
        player->current_level--;
    } else if (player->buffer_health > BUFFER_SIZE * 3 / 4 && player->current_level < player->num_levels - 1) {
        player->current_level++;
    }
}

void simulate_playback(AdaptivePlayer *player) {
    for (int i = 0; i < 100; i++) {
        int network_speed = rand() % 10000 + 1000;
        
        player->buffer_health += network_speed / 1000;
        player->buffer_health -= player->levels[player->current_level].bitrate / 1000;
        
        if (player->buffer_health < 0) player->buffer_health = 0;
        if (player->buffer_health > BUFFER_SIZE) player->buffer_health = BUFFER_SIZE;
        
        adapt_quality(player);
        
        printf("Time: %ds, Quality: %d kbps, Buffer: %d\n", 
               i, player->levels[player->current_level].bitrate, player->buffer_health);
        
        usleep(100000);
    }
}

int main() {
    AdaptivePlayer player;
    init_player(&player);
    
    add_quality_level(&player, 500, "test_1.mp4");
    add_quality_level(&player, 1000, "test_2.mp4");
    add_quality_level(&player, 2000, "test_3.mp4");
    add_quality_level(&player, 4000, "test_4.mp4");
    
    simulate_playback(&player);
    
    return 0;
}
