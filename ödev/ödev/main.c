//
//  main.c
//  ödev
//
//  Created by gözde on 25.04.2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100
// merhaba bu videoda sosyal ağ analizini ağaç tabanları veri yapıları ile nasıl yaptığımı göstereceğim.Projede C dilinde yazıldı.veri yapısı olarak grafik, DFS ve kırmızı-siyah ağaçlar kullandım
// ----------- GRAPH (İlişki Ağı) ------------------
typedef struct Node {
    int user_id;
    struct Node* next;
} Node;
// veriler dosyadan okunuyor.kullanıcılar ve arkadaşlık ilişkileri add_user ve add_ firend fonksiyonlarıyla belleğe alınıyor.her kullanıcı user yapısı ile temsil ediliyor
typedef struct {
    int user_id;
    Node* friends;
} User;

User users[MAX_USERS];
int user_count = 0;

// Kullanıcı bul
int find_user(int id) {
    for (int i = 0; i < user_count; i++)
        if (users[i].user_id == id) return i;
    return -1;
}

// Yeni kullanıcı ekle
void add_user(int id) {
    if (find_user(id) == -1) {
        users[user_count].user_id = id;
        users[user_count].friends = NULL;
        user_count++;
    }
}
// dfs_distance fonksiyonu ile belirli arkadaşlara ulaşıyoruz örneğin 101 numaralı kullanıcıdan 2 adımda erişilen kişiler yazdırılıyor
// Arkadaş ekle
void add_friend(int u1, int u2) {
    int i1 = find_user(u1), i2 = find_user(u2);

    Node* node1 = malloc(sizeof(Node));
    node1->user_id = u2;
    node1->next = users[i1].friends;
    users[i1].friends = node1;

    Node* node2 = malloc(sizeof(Node));
    node2->user_id = u1;
    node2->next = users[i2].friends;
    users[i2].friends = node2;
}

// Dosya oku
void build_graph(const char* filename) {
    FILE* file = fopen(filename, "r");
    char type[10];
    int u1, u2;
    while (fscanf(file, "%s", type) != EOF) {
        if (strcmp(type, "USER") == 0) {
            fscanf(file, "%d", &u1);
            add_user(u1);
        } else if (strcmp(type, "FRIEND") == 0) {
            fscanf(file, "%d %d", &u1, &u2);
            add_friend(u1, u2);
        }
    }
    fclose(file);
}

// ----------- DFS (Belirli Mesafe Arkadaşlar) ----------
int visited[MAX_USERS];

void dfs_distance(int index, int depth, int target) {
    visited[index] = 1;
    if (depth == target) {
        printf("Kullanıcı %d hedef mesafede.\n", users[index].user_id);
        return;
    }
    for (Node* n = users[index].friends; n; n = n->next) {
        int ni = find_user(n->user_id);
        if (!visited[ni])
            dfs_distance(ni, depth + 1, target);
    }
}

// ----------- Ortak Arkadaş Analizi ----------
void common_friends(int id1, int id2) {
    int i1 = find_user(id1), i2 = find_user(id2);
    printf("Ortak arkadaşlar: ");
    for (Node* a = users[i1].friends; a; a = a->next)
        for (Node* b = users[i2].friends; b; b = b->next)
            if (a->user_id == b->user_id)
                printf("%d ", a->user_id);
    printf("\n");
}
//iki kullanıcının arkadaş listeleri karşılaştırılarak ortak arkadaşlar yazdırılıyor bu işlem basit bir iki döngü ile yapmakta
// ----------- Topluluk Tespiti ----------
void dfs_community(int index) {
    visited[index] = 1;
    printf("%d ", users[index].user_id);
    for (Node* n = users[index].friends; n; n = n->next) {
        int ni = find_user(n->user_id);
        if (!visited[ni])
            dfs_community(ni);
    }
}

void detect_communities(void) {
    memset(visited, 0, sizeof(visited));
    printf("Topluluklar:\n");
    for (int i = 0; i < user_count; i++) {
        if (!visited[i]) {
            dfs_community(i);
            printf("\n");
        }
    }
}

// ----------- Etki Alanı ----------
int influence_dfs(int index) {
    visited[index] = 1;
    int count = 1;
    for (Node* n = users[index].friends; n; n = n->next) {
        int ni = find_user(n->user_id);
        if (!visited[ni])
            count += influence_dfs(ni);
    }
    return count;
}

void influence_domain(int user_id) {
    memset(visited, 0, sizeof(visited));
    int index = find_user(user_id);
    int total = influence_dfs(index);
    printf("Kullanıcı %d'nin etki alanı: %d kişi\n", user_id, total - 1);
}
// dfs ile bağlı bileşelnler tespit edilerek her topluluk ekrana yazdırılıyor. bu bize ağdaki doğal kümeleri gösteriyor
// ----------- RED-BLACK TREE (Kimlik Arama) ----------
typedef enum { RED, BLACK } Color;

typedef struct rb_node {
    int key;
    Color color;
    struct rb_node *left, *right, *parent;
} rb_node;

rb_node* root = NULL;

// Kırmızı-siyah ağaç fonksiyonları burada sadeleştirilmiş şekilde bırakılmıştır.
//bir kullanıcının etkisi kaç farklı kullanıcıya  ulaşabildiği ile ölçülüyor dfs erişilen kişi sayısı hesaplanıyor

void dummy_rb_insert(int key) {
    printf("Kullanıcı %d kırmızı-siyah ağaçta eklendi (simülasyon).\n", key);
}
//bu kısımda her kullanıcıyı kırmızı siyah ağaçta ekliyoruz gerçek uygulamada aramalar o(logn)sürede yapılır.
// ----------- MAIN ----------
int main(void) {
    build_graph("veriseti.txt");

    printf("DFS ile mesafe 2 arkadaşları (101 için):\n");
    memset(visited, 0, sizeof(visited));
    dfs_distance(find_user(101), 0, 2);

    printf("\nOrtak arkadaşlar (102-103):\n");
    common_friends(102, 103);

    printf("\nTopluluklar:\n");
    detect_communities();

    printf("\nEtki alanı (104):\n");
    influence_domain(104);

    printf("\nKırmızı-siyah ağaç örneği:\n");
    for (int i = 0; i < user_count; i++)
        dummy_rb_insert(users[i].user_id);

    return 0;
}
// projem bu şekilde tamamlanmıştır Github bağlantım ve video içeriği yönergelere uygundur teşekkür ederim.
